#version 430

//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;

//out vector needed for the fragment shader
out vec3 VPosition;
out vec3 VNormal;


//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 ProjectionMatrix;   // projection matrix

 
void main() 
{ 
   VNormal = normalize(NormalMatrix * VertexNormal);
   VPosition = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

   gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(VertexPosition, 1.0);
}
