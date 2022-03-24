#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;
in vec2 TexCoord;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;

layout (binding=0) uniform sampler2D Tex1;
//layout (binding= 0) uniform sampler2D HdrTex;
layout (binding = 1) uniform sampler2D BlurTex1;
layout (binding = 2) uniform sampler2D BlurTex2;


uniform float EdgeThreshold;
uniform int Pass;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

uniform struct LightInfo 
{
  vec4 Position; // Light position in eye coords.
  vec3 Ld;       // Diffuse light intensity
  vec3 La;
  vec3 Ls;


} Light;

//material information struct
uniform struct MaterialInfo 
{
  vec3 Kd;      // Diffuse reflectivity
  vec3 Ks;
  vec3 Ka;
  float shininess;
} Material;

uniform float LumThresh; // luminence threshold

uniform float PixOffset[10] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0);
uniform float Weight[10];

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

vec3 BlinnPhongModel(int light, vec3 Position, vec3 n)
{
    vec3 texColor = texture(Tex1,TexCoord).rgb;


    //ambient component  
    vec3 Ambient = texColor * Light.La;

    //calculate dot product for vector s and n using max. 
    float sDotN = max( dot(s,n), 0.0 );

    //difuse formula for light calculations
    vec3 diffuse = Light.Ld * texColor * sDotN;
 
    //specular component
    vec3 Specular = texColor * Light.Ls * pow(max(dot(r,v), 1.0), Material.shininess); 


    return diffuse + Ambient + Specular;
}


void main()
{
    //we pass LightIntensity to our FragColor, notice the difference between vector types
    // vec3 and vec4 and how we solved the problem
    if(Pass == 1) FragColor = pass1();
    if (Pass == 2) FragColor = pass2();

    FragColor = vec4(LightIntensity, 1.0);
}

vec4 pass1()
{
    return vec4(blinnPhong( Position, normalize(Normal) ), 1.0);
}

vec4 pass2()
{
    ivec2 pix = ivec2(gl_FragCoord.xy); //we grab a pixel to check if edge
    //pick neighboutring pixels for convolution filter
    //check lecture slides
    float s00 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,1)).rgb);
    float s10 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,0)).rgb);
    float s20 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(-1,-1)).rgb);
    float s01 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(0,1)).rgb);
    float s21 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(0,-1)).rgb);
    float s02 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,1)).rgb);
    float s12 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,0)).rgb);
    float s22 = luminance(texelFetchOffset(RenderTex, pix, 0, ivec2(1,-1)).rgb);
    float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
    float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);
    float g = sx * sx + sy * sy;
    if( g > EdgeThreshold )
    return vec4(1.0); //edge
    else
    return vec4(0.0,0.0,0.0,1.0); //no edge
}

