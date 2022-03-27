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
  vec4 LightPosition; // Light position in eye coords.
  vec3 Ld;       // Diffuse light intensity
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

uniform struct FogInfo
{
    float MaxDist;
    float MinDist;
    vec3 FogColor;
} Fog;


vec3 BlinnPhong(vec4 position, vec3 n, int light)
{


     //retrieve texture color from texture
     vec3 TexColor = texture(ColorTex, TexCoord).rgb;
     vec4 TexNormal = texture(NormalMapTex, TexCoord);

     //Mix Brick and Moss textures together, using alpha channel
     //vec3 col = mix(BrickTexColor.rgb, MossTexColor.rgb, MossTexColor.a);

     //vec4 texColor = texture(BaseTex,TexCoord).rgb;

     //ambient component
     vec3 Ambient = TexColor * Light.La;   

     //calculate light direction
     //vec3 s = normalize(vec3(Light.Position - position));

      //calculate dot product for vector s and n using max.
     float sDotN = max( dot(LightDir,n), 0.0 );

     //difuse formula for light calculations
     vec3 diffuse = Light.Ld * TexColor * sDotN;

     //reflect vector
     vec3 r = reflect(-LightDir,n);

     //viewing vector
     //vec3 v = normalize(-position.xyz);

     //half vector
     vec3 h = normalize(ViewDir + LightDir);

     //specular component
     vec3 Specular = TexColor * Light.Ls * pow(max(dot(h,n), 1.0), Material.shininess); 
       
     //phong calculation
     vec3 LightIntensity = diffuse + Ambient + Specular;
     return LightIntensity;
}

void main()
{   
    //calculate fog
    float dist = abs(position.z);
    float fogFactor = (Fog.MaxDist - dist) / (Fog.MaxDist - Fog.MinDist);

    fogFactor = clamp(fogFactor, 0.0, 1.0);

    //get BlinnPhong calculation
    vec3 shadeColor = BlinnPhong(position, normalize(normal),1);

    //combine color of Fog, BlinnPhong and fog density
    vec3 color = mix(Fog.FogColor, shadeColor, fogFactor);

    //mapping normal onto texture
    vec3 normal = texture(NormalMapTex, TexCoord).xyz;
    normal.xy = 2.0 * normal.xy - 1.0;

    //output
    FragColor = vec4(color, 1.0);
    //FragColor = vec4(BlinnPhong(position, normalize(normal), 1), 1.0);
}



