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
    vec3 position;
    vec3 direction;
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
    vec3 lightDir = normalize(light.position.xyz - FragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    if(theta > light.cutOff)
    {
        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).xyz;

        vec3 norm = normalize(Normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).xyz;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).xyz;

        // point light attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        diffuse *= attenuation;
        specular *= attenuation;
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result * 3, 1.0);
    } else {
        FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb * 3, 1.0);
    }
}
