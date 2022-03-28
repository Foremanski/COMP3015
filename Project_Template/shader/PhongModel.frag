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

    

    //output
    FragColor = vec4(color, 1.0);
    //FragColor = vec4(BlinnPhong(position, normalize(normal), 1), 1.0);
}



