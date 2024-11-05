#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec4 color;
uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 view_pos;

void main()
{
    // ambient
    float ambientStrength = 0.9;
    vec3 ambient = ambientStrength * light_color;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light_pos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light_color;

    // specular
    float specular_strength = 0.3;
    vec3 viewDir = normalize(view_pos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * vec3(color);
    FragColor = vec4(result, color.a);
}