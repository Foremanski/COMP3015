#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;
in vec4 position;
in vec3 normal;

in vec2 TexCoord;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;


layout (binding = 0) uniform sampler2D BrickTex;
layout (binding = 1) uniform sampler2D MossTex;


//light information structure
uniform struct LightInfo 
{
  vec4 Position; // Light position in eye coords.
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


vec3 BlinnPhong(vec4 position, vec3 n, int light)
{


     //retrieve texture color from texture
     vec3 BrickTexColor = texture(BrickTex, TexCoord).rgb;
     vec4 MossTexColor = texture(MossTex, TexCoord);

     //Mix Brick and Moss textures together, using alpha channel
     vec3 col = mix(BrickTexColor.rgb, MossTexColor.rgb, MossTexColor.a);

     //vec4 texColor = texture(BaseTex,TexCoord).rgb;

     //ambient component
     vec3 Ambient = col * Light.La;   

     //calculate light direction
     vec3 s = normalize(vec3(Light.Position - position));

      //calculate dot product for vector s and n using max.
     float sDotN = max( dot(s,n), 0.0 );

     //difuse formula for light calculations
     vec3 diffuse = Light.Ld * col * sDotN;

     //reflect vector
     vec3 r = reflect(-s,n);

     //viewing vector
     vec3 v = normalize(-position.xyz);

     //half vector
     vec3 h = normalize(v + s);

     //specular component
     vec3 Specular = col * Light.Ls * pow(max(dot(h,n), 1.0), Material.shininess); 
       
     //phong calculation
     vec3 LightIntensity = diffuse + Ambient + Specular;
     return LightIntensity;
}

void main()
{   
    //vec4 alphaMap = texture(AlphaTex, TexCoord);

    //if(alphaMap.a < 0.15)
    //    discard;
    //else
    //{
     //   if(gl_FrontFacing)
            FragColor = vec4(BlinnPhong(position, normalize(normal), 1), 1.0);
    //    else
    //        FragColor = vec4(BlinnPhong(position, normalize(-normal), 1), 1.0);
   // }
}



