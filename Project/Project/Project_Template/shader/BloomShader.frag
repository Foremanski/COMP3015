#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec4 Position;
in vec3 Normal;

in vec2 TexCoord;
in vec3 LightDir;
in vec3 ViewDir;


layout (binding = 0) uniform sampler2D RenderTex;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;

uniform float EdgeThreshold;
uniform int Pass;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);

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
//fog structure


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

float luminance(vec3 color)
{
    return dot(lum, color);
}

vec4 pass1()
{
    return vec4(BlinnPhong(Position,normalize(Normal)), 1.0f);
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
    {
        return vec4(1.0); //edge
    }
    
    else
    {
        return vec4(0.0,0.0,0.0,1.0); //no edge
    }  
}


void main()
{   
    if( Pass == 1 ) FragColor = pass1();
    if( Pass == 2 ) FragColor = pass2(); 

}
