#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;


//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;

void main()
{
    //we pass LightIntensity to our FragColor, notice the difference between vector types
    // vec3 and vec4 and how we solved the problem
    FragColor = vec4(LightIntensity, 1.0);
}
