#version 430 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct Light {
    vec4 direction_or_position;//look
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 fragPos;
in vec2 TexCoords;

out vec4 FragColor;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambientLight = vec3(texture(material.diffuse, TexCoords)) * light.ambient;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir; 
	
	if(light.direction_or_position.w == 0.0)
		lightDir = normalize(-vec3(light.direction_or_position));
	else
		lightDir = normalize(vec3(light.direction_or_position) - fragPos);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
	
	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * light.specular;

	vec3 emission = vec3(0.0, 0.0, 0.0);
	
	if(vec3(texture(material.specular, TexCoords)) == vec3(0.0, 0.0, 0.0))
		emission = vec3(texture(material.emission, TexCoords));
	
    FragColor = vec4((ambientLight + diffuse + specular), 1.0);
}