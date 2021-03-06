#version 430

//in variables, this are in model coordinates
layout (location = 0) in vec3 VertexPosition; 
layout (location = 1) in vec3 VertexNormal; 


//out vector needed for the fragment shader
out vec3 position;
out vec3 normal;


//uniforms for matrices required in the shader
uniform mat4 ModelViewMatrix;   //model view matrix
uniform mat3 NormalMatrix;		//normal matrix
uniform mat4 ProjectionMatrix;   // projection matrix
uniform mat4 MVP;				//model view projection matrix

uniform struct LightInfo 
{
  vec4 LightPosition; // Light position 
  vec3 La;       // Ambient Light intensity
  vec3 L;
} Light;
 
void main() 
{ 
   normal = normalize(NormalMatrix * VertexNormal);
   position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;

   gl_Position = MVP * vec4(VertexPosition,1.0);





    /*
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

    //turns any vertex position into model view projection in preparations to 
    //graphics pipeline processes before fragment shader (clipping)
    gl_Position = MVP * vec4(VertexPosition,1.0); 
    */
} 