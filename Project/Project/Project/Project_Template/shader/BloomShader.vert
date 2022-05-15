#version 430

//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;

//out vector needed for the fragment shader
out vec4 Position;
out vec3 Normal;
out vec2 TexCoord;


//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
//uniform mat4 ProjectionMatrix;   // projection matrix
uniform mat4 MVP;				//model view projection matrix
 
void main() 
{ 
	vec4 pos = vec4(VertexPosition, 1.0);
	
	vec3 n = vec3(0.0);
	
	Position = ModelViewMatrix * pos;
	Normal = normalize(NormalMatrix * n);
	TexCoord = VertexTexCoord;

	gl_Position = MVP * pos;

} 