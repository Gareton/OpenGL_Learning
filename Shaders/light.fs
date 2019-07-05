#version 430 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct Light {
    vec3 position;
 //   vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;
in vec3 fragPos;
in vec2 TexCoords;

out vec4 FragColor;
  
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
	vec3 ambientLight = lightColor * vec3(texture(material.diffuse, TexCoords)) * light.ambient;
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - fragPos);
	//vec3 lightDir = normalize(-light.direction); 
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * vec3(texture(material.diffuse, TexCoords)) * light.diffuse;
	
	vec3 viewDir = normalize(-fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * lightColor * light.specular;

	vec3 emission = vec3(0.0, 0.0, 0.0);
	
	if(vec3(texture(material.specular, TexCoords)) == vec3(0.0, 0.0, 0.0))
		emission = vec3(texture(material.emission, TexCoords));
	
    FragColor = vec4((ambientLight + diffuse + specular + emission), 1.0);
}