#version 430

//in variable that receives the diffuse calculation from the vertex shader
in vec4 Position;
in vec3 Normal;

//out variable, this typical for all fragment shaders
layout (location = 0) out vec4 FragColor;

//light information structure
uniform struct LightInfo 
{
  vec4 LightPosition;      
  vec3 La;
  vec3 L;
} Light;

//material information structure
uniform struct MaterialInfo 
{
  vec3 Kd;
  vec3 Ks;
  vec3 Ka;
  float shininess;
} Material;

vec3 BlinnPhong( vec4 position, vec3 n )
{
    vec3 Ambient = (Material.Ka * Light.La);
    vec3 s = (Light.LightPosition - (Position * Light.LightPosition.w)).xyz;
    float sDotN = max( dot(s , Normal), 0.0 );

    vec3 Diffuse = Material.Kd * sDotN;
    vec3 Specular = vec3(0.0);

    if(sDotN > 0.0)
    {
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        Specular = Material.Ks * pow (max (dot (h,n) , 0.0), Material.shininess);
    }
    //phong calculation
    return Ambient + Light.L * (Diffuse + Specular);
}

void main()
{   
    
    FragColor = vec4(BlinnPhong(Position, normalize(Normal)), 1.0f);
}
