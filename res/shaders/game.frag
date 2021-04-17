#version 410 core
out vec4 FragColor;
in vec3 normal;
in vec3 colorNormal;
  
void main()
{
    vec3 lightPos = normalize(vec3(1.0, 1.0, 1.0));
    float intensity = max(dot(lightPos, normal), 0.0) + 0.2;
    vec3 color = 0.5 * colorNormal + 0.5;
    color *= intensity;
    FragColor = vec4(color, 1.0);
}