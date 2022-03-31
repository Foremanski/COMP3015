#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;
in vec4 position;
in vec3 normal;

in vec2 TexCoord;
in vec3 LightDir;
in vec3 ViewDir;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;


layout (binding = 0) uniform sampler2D ColorTex;
layout (binding = 1) uniform sampler2D NormalMapTex;
layout (binding = 2) uniform sampler2D RenderTex;

uniform float EdgeThreshold;
uniform int Pass;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);


//light information structure
uniform struct LightInfo 
{
  vec4 LightPosition; 
  vec3 Ld;      
  vec3 La;
  vec3 Ls;
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
uniform struct FogInfo
{
    float MaxDist;
    float MinDist;
    vec3 FogColor;
} Fog;
//spotlight structure
uniform struct SpotLightInfo {
    vec3 SpotPosition;
    vec3 L;
    vec3 La;
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spot;



vec3 BlinnPhong(vec4 position, vec3 n, int light)
{
     //retrieve texture color from texture
     vec3 TexColor = texture(ColorTex, TexCoord).rgb;
     vec4 TexNormal = texture(NormalMapTex, TexCoord);

     //ambient component
     vec3 Ambient = TexColor * Light.La;   

     //spotlight variables
     float cosAng = dot(-LightDir, normalize(Spot.Direction));
     float angle = acos (cosAng);
     float spotScale = 0.0;

     //calculate dot product for vector s and n using max
     float sDotN = max( dot(LightDir,n), 0.0 );

     //difuse formula for light calculations
     vec3 Diffuse = Light.Ld * TexColor * sDotN;

     //reflect vector
     vec3 r = reflect(-LightDir,n);

     //half vector
     vec3 h = normalize(ViewDir + LightDir);
              
     //specular component
     vec3 Specular = TexColor * Light.Ls * pow(max(dot(h,n), 1.0), Material.shininess); 
     
     //calculate spotlight if angle of light is less than cutoff
     if(angle < Spot.Cutoff)
     {
         spotScale = pow(cosAng, Spot.Exponent);
     }            
          
     //phong calculation
     vec3 LightIntensity = Ambient + spotScale + Spot.L * (Diffuse + Specular);
     return LightIntensity;
}

float luminance(vec3 color)
{
    return dot(lum, color);
}

vec4 pass1()
{
    return vec4(BlinnPhong(position, normalize(normal), 1), 1);
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



void main()
{   
    //calculate fog
    float dist = abs(position.z);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    //mapping normal onto texture
    vec3 normal = texture(NormalMapTex, TexCoord).xyz;
    normal.xy = 2.0 * normal.xy - 1.0;

    //get BlinnPhong calculation
    vec3 shadeColor = BlinnPhong(position, normalize(normal),1);

    //combine color of Fog, BlinnPhong and fog density
    vec3 color = mix(Fog.FogColor, shadeColor, fogFactor);

    if( Pass == 1) FragColor = pass1();
    if (Pass == 2) FragColor = pass2();

    //output
    FragColor = vec4(color, 1.0);
}




