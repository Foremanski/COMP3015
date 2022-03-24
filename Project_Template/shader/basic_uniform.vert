#version 430

//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;

//out vector needed for the fragment shader
out vec4 position;
out vec3 normal;
out vec2 TexCoord;
 
 //light information struct


//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 MVP;				//model view projection matrix
 
void main() 
{ 
  //pass VertexTexCoord to TexCoord
  TexCoord = VertexTexCoord;
	
  //transfrom normal from model coordinates to view coordinates
  vec3 n = normalize(NormalMatrix * VertexNormal);

  //transform vertex position from model coordinates to view coordinates
  position = ModelViewMatrix * vec4(VertexPosition,1.0);

  //calculate light direction, notice the light is already in the view coordinates 
  vec3 s = normalize(vec3(Light.Position - pos));

  //reflect vector
  vec3 r = reflect(s,n);

  //viewing vector
  vec3 v = normalize(-VertexPosition.xyz);


  //turns any vertex position into model view projection in preparations to 
  //graphics pipeline processes before fragment shader (clipping)
  gl_Position = MVP * vec4(VertexPosition,1.0); 
} 