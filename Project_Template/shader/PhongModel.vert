#version 430

//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 
layout (location = 2) in vec2 VertexTexCoord;
layout (location = 3) in vec4 VertexTangent;

//out vector needed for the fragment shader
out vec3 LightDir;
out vec3 ViewDir;
out vec3 position;
out vec3 normal;
out vec2 TexCoord;

//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;				//model view projection matrix

uniform struct LightInfo 
{
  vec4 LightPosition; // Light position in eye coords.
  vec3 Ld;       // Diffuse light intensity
  vec3 La;
  vec3 Ls;
} Light;
 
void main() 
{ 
  
    

    //transfrom normal from model coordinates to view coordinates
    normal = normalize(NormalMatrix * VertexNormal);
    vec3 tangent = normalize(NormalMatrix * vec3(VertexTangent));

    //binormal computation
    vec3 binormal = normalize( cross(normal,tangent)) * VertexTangent.w;

    mat3 toObjectLocal = mat3 (
    tangent.x, binormal.x, normal.x,
    tangent.y, binormal.y, normal.y,
    tangent.z, binormal.z, normal.z);

    //transform light direction and view direction to tangent space
    vec3 pos = vec3( ModelViewMatrix * vec4(VertexPosition, 1.0));
    LightDir = toObjectLocal * (Light.LightPosition.xyz - pos);
    ViewDir = toObjectLocal * normalize(-pos);

    //pass VertexTexCoord to TexCoord
    TexCoord = VertexTexCoord;

    //transform vertex position from model coordinates to view coordinates
    //position = (ModelViewMatrix * vec4(VertexPosition,1.0)).xyz;

    //turns any vertex position into model view projection in preparations to 
    //graphics pipeline processes before fragment shader (clipping)
    gl_Position = MVP * vec4(VertexPosition,1.0); 
} 