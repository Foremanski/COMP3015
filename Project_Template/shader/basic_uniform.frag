#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec3 LightIntensity;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;


layout (binding= 0) uniform sampler2D HdrTex;
layout (binding = 1) uniform sampler2D BlurTex1;
layout (binding = 2) uniform sampler2D BlurTex2;

uniform float EdgeThreshold;
uniform int Pass;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);
uniform vec4 Position;
uniform vec3 L;
uniform vec3 La;

uniform float LumThresh; // luminence threshold

uniform float PixOffset[10] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0);
uniform float Weight[10];

uniform mat3 rgb2xyz = mat3(
 0.4124564, 0.2126729, 0.0193339,
 0.3575761, 0.7151522, 0.1191920,
 0.1804375, 0.0721750, 0.9503041 );
uniform mat3 xyz2rgb = mat3(
 3.2404542, -0.9692660, 0.0556434,
 -1.5371385, 1.8760108, -0.2040259,
 -0.4985314, 0.0415560, 1.0572252 );


uniform float Exposure = 0.35;
uniform float White = 0.928;
uniform float AveLum;

void main()
{
    //we pass LightIntensity to our FragColor, notice the difference between vector types
    // vec3 and vec4 and how we solved the problem
    if(Pass == 1) FragColor = pass1();
    if (Pass == 2) FragColor = pass2();

    FragColor = vec4(LightIntensity, 1.0);
}

vec4 pass1()
{
    return vec4(blinnPhong( Position, normalize(Normal) ), 1.0);
}

vec4 pass2()
{
    
}

