#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;
in vec2 TexCoord;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;


layout (binding = 0) uniform sampler2D Tex1;

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
  float shininess;
} Material;



void main()
{
    
    
    FragColor = vec4(LightIntensity, 1.0);
}

vec3 BlinnPhong(int light, vec4 position, vec3 n)
{
     //retrieve texture color from texture
     vec3 texColor = texture(Tex1,TexCoord).rgb;

     //calculate light direction, notice the light is already in the view coordinates 
     vec3 s = normalize(vec3(Light.Position - position));

      //calculate dot product for vector s and n using max. Read about max in glsl documentation, if not clear talk to me
     float sDotN = max( dot(s,n), 0.0 );

        //difuse formula for light calculations
     vec3 diffuse = Light.Ld * texColor * sDotN;

     //reflect vector
     vec3 r = reflect(-s,n);

     //viewing vector
     vec3 v = normalize(-position.xyz);

     //ambient component
     vec3 Ambient = texColor * Light.La;

     //specular component
     vec3 Specular = texColor * Light.Ls * pow(max(dot(r,v), 1.0), Material.shininess); 

     //phong calculation
     vec3 LightIntensity = diffuse + Ambient + Specular;

     return LightIntensity;
}



