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
} Light;

//material information structure
uniform struct MaterialInfo 
{
  vec3 Kd;
  vec3 Ks;
  vec3 Ka;
  float shininess;
} Material;


uniform int Pass;
uniform float AveLum;

layout (binding = 0) uniform sampler2D HdrTex;

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
uniform bool DoToneMap = true;



vec3 BlinnPhong(vec4 position, vec3 n)
{
    vec3 Ambient = (Material.Ka * Light.La);
    vec3 s = (Light.LightPosition - (Position * Light.LightPosition.w)).xyz;
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
    return Ambient + Light.L * (Diffuse + Specular);
}

void pass1()
{
    vec3 n = normalize(Normal);

    HdrColor = vec3(0.0);
    for(int i = 0; i < 3; i++)
    {
        HdrColor += BlinnPhong(Position, n), i;
    }
}

void pass2()
{
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

     // Convert back to RGB and send to output buffer
    if( DoToneMap )
    FragColor = vec4( xyz2rgb * xyzCol, 1.0);
    else
    FragColor = color;
}

void main()
{   
    if( Pass == 1 ) pass1();
    else if( Pass == 2 ) pass2(); 
}
