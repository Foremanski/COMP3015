#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec4 Position;
in vec3 Normal;
in vec2 TexCoord;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 HdrColor;


//light information structure
uniform struct LightInfo 
{
  vec4 LightPosition;      
  vec3 La;
  vec3 L;
} Lights[3];

//material information structure
uniform struct MaterialInfo 
{
  vec3 Kd;
  vec3 Ks;
  vec3 Ka;
  float shininess;
} Material;


uniform int Pass;

layout (binding = 0) uniform sampler2D HdrTex;
layout (binding = 1) uniform sampler2D BlurTex1;
layout (binding = 2) uniform sampler2D BlurTex2;

uniform float LumThresh;

uniform float PixOffset[10] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0);
uniform float Weight[10];


//RGB/XYZ conversion
uniform mat3 rgb2xyz = mat3(
 0.4124564, 0.2126729, 0.0193339,
 0.3575761, 0.7151522, 0.1191920,
 0.1804375, 0.0721750, 0.9503041 );

uniform mat3 xyz2rgb = mat3(
 3.2404542, -0.9692660, 0.0556434,
 -1.5371385, 1.8760108, -0.2040259,
 -0.4985314, 0.0415560, 1.0572252 );

uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform float AveLum;

float luminance(vec3 color)
{
    float r = 0.2126;
    float b = 0.0722;
    float g = 0.7152;

    return  r * color.r + g * color.g + b * color.b;
}

vec3 BlinnPhong( vec4 position, vec3 n, int Light)
{
    vec3 Ambient = (Material.Ka * Lights[Light].La);
    vec3 s = (Lights[Light].LightPosition - (Position * Lights[Light].LightPosition.w)).xyz;
    float sDotN = max( dot(s , Normal), 0.0 );

    vec3 Diffuse = Material.Kd * sDotN;
    vec3 Specular = vec3(0.0);

    if(sDotN > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        Specular = Material.Ks * pow (max (dot (h,n) , 0.0), Material.shininess);
    }
    //phong calculation
    return Ambient + Lights[Light].L * (Diffuse + Specular);
}



vec4 pass1()
{
    vec3 n = normalize(Normal);

    vec3 color = vec3(0.0);

    for(int i = 0; i < 3; i++)
    {
        color += BlinnPhong(Position, n, i);
    }

    return vec4(color,1);
}

// Bright-pass filter (write to BlurTex1)
vec4 pass2()
{   
    vec4 val = texture(HdrTex, TexCoord);

    if( luminance(val.rgb) > LumThresh)
    {
        return val;
    }
    
    else
    {
        return vec4(0.0);
    }    
}
//first blur pass
vec4 pass3()
{
    float dy = 1.0 / (textureSize(BlurTex1,0)).y;
    vec4 sum = texture(BlurTex1, TexCoord) * Weight[0];

    for( int i = 1; i < 10; i++)
    {
        sum += texture( BlurTex1, TexCoord + vec2(0.0,PixOffset[i]) * dy ) * Weight[i];
        sum += texture( BlurTex1, TexCoord - vec2(0.0,PixOffset[i]) * dy ) * Weight[i];
    }

    return sum;
}

// Second blur (read from BlurTex2, write to BlurTex1)
vec4 pass4()
{
    float dx = 1.0 / (textureSize(BlurTex2,0)).x;
    vec4 sum = texture(BlurTex2, TexCoord) * Weight[0];

    for( int i = 1; i < 10; i++ )
    {
        sum += texture( BlurTex2, TexCoord + vec2(PixOffset[i],0.0) * dx ) * Weight[i];
        sum += texture( BlurTex2, TexCoord - vec2(PixOffset[i],0.0) * dx ) * Weight[i];
    }
     return sum;
}

// (Read from BlurTex1 and HdrTex, write to default buffer).
vec4 pass5()
{
    /////////////// Tone mapping ///////////////
    // Retrieve high-res color from texture
    vec4 color = texture( HdrTex, TexCoord );

    // Convert to XYZ
    vec3 xyzCol = rgb2xyz * vec3(color);

    // Convert to xyY
    float xyzSum = xyzCol.x + xyzCol.y + xyzCol.z;
    vec3 xyYCol = vec3( xyzCol.x / xyzSum, xyzCol.y / xyzSum, xyzCol.y);

    // Apply the tone mapping operation to the luminance (xyYCol.z or xyzCol.y)
    float L = (Exposure * xyYCol.z) / AveLum;
    L = (L * ( 1 + L / (White * White) )) / ( 1 + L );

    // Using the new luminance, convert back to XYZ
    xyzCol.x = (L * xyYCol.x) / (xyYCol.y);
    xyzCol.y = L;
    xyzCol.z = (L * (1 - xyYCol.x - xyYCol.y))/xyYCol.y;

    // Convert back to RGB
    vec4 toneMapColor = vec4( xyz2rgb * xyzCol, 1.0);

    ///////////// Combine with blurred texture /////////////
    // We want linear filtering on this texture access so that
    // we get additional blurring.
    vec4 blurTex = texture(BlurTex1, TexCoord);

    return toneMapColor + blurTex;
}

void main()
{   
    if(Pass == 1) FragColor = pass1();
    else if(Pass == 2) FragColor = pass2();
    else if(Pass == 3) FragColor = pass3();
    else if(Pass == 4) FragColor = pass4();
    else if(Pass == 5) FragColor = pass5();

}
