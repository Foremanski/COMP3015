#version 430

//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 

//out vector needed for the fragment shader
out vec3 LightIntensity; 
 
 //light information struct
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

//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 MVP;				//model view projection matrix
 
void main() 
{ 
  //transfrom normal from model coordinates to view coordinates
  vec3 n = normalize(NormalMatrix * VertexNormal);

  //transform vertex position from model coordinates to view coordinates
  vec4 pos = ModelViewMatrix * vec4(VertexPosition,1.0);

  //calculate light direction, notice the light is already in the view coordinates 
  vec3 s = normalize(vec3(Light.Position - pos));

  //calculate dot product for vector s and n using max. Read about max in glsl documentation, if not clear talk to me
  float sDotN = max( dot(s,n), 0.0 );

  //difuse formula for light calculations
  vec3 diffuse = Light.Ld * Material.Kd * sDotN;

  //reflect vector
  vec3 r = reflect(s,n);

  //viewing vector
  vec3 v = normalize(-VertexPosition.xyz);

  //ambient component
  vec3 Ambient = Material.Ka * Light.La;

  //specular component
  vec3 Specular = Material.Ks * Light.Ls * pow(dot(r,v), Material.shininess); 

  //phong calculation
  LightIntensity = diffuse + Ambient + Specular;

  //turns any vertex position into model view projection in preparations to 
  //graphics pipeline processes before fragment shader (clipping)
  gl_Position = MVP * vec4(VertexPosition,1.0); 
} 