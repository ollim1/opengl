#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec4 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

void main()
{
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).xyz;

    vec3 norm = normalize(Normal);
    vec3 lightDir;
    if (light.direction.w == 0.0)
        lightDir = normalize((-light.direction).xyz);
    else
        lightDir = normalize(light.direction.xyz - FragPos);


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).xyz;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).xyz;

    if (light.direction.w != 0.0) {
        // point light attenuation
        float distance = length(light.direction.xyz - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
    }
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result * 3, 1.0);
}
