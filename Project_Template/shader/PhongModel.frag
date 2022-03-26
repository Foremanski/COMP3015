#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;
in vec4 position;
in vec3 normal;

//in vec2 TexCoord;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;


//layout (binding = 0) uniform sampler2D Tex1;

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
     //ambient component
     vec3 Ambient = Material.Ka * Light.La;   

     //calculate light direction, notice the light is already in the view coordinates 
     vec3 s = normalize(vec3(Light.Position - position));

      //calculate dot product for vector s and n using max. Read about max in glsl documentation, if not clear talk to me
     float sDotN = max( dot(s,n), 0.0 );

     //difuse formula for light calculations
     vec3 diffuse = Light.Ld * Material.Kd * sDotN;

     //reflect vector
     vec3 r = reflect(-s,n);

     //viewing vector
     vec3 v = normalize(-position.xyz);


     //half vector
     vec3 h = normalize(v + s);

     //specular component
     vec3 Specular = Material.Ks * Light.Ls * pow(max(dot(h,n), 1.0), Material.shininess); 

     //retrieve texture color from texture
     //vec3 texColor = texture(Tex1,TexCoord).rgb;         
       
     //phong calculation
     vec3 LightIntensity = diffuse + Ambient + Specular;

     return LightIntensity;
}

void main()
{   
    FragColor = vec4(BlinnPhong(position, normalize(normal), 1), 1.0);

}



