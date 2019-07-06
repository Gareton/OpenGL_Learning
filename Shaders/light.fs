#version 430 core

struct Material{
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct Light {
    vec4 direction_or_position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

in vec3 Normal;
in vec3 fragPos;
in vec2 TexCoords;

out vec4 FragColor;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

float sq(float val);

void main()
{
	//ambient
	vec3 ambient = vec3(texture(material.diffuse, TexCoords)).rgb * light.ambient;
	
	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir; 
	
	if(light.direction_or_position.w == 0.0)
		lightDir = normalize(-vec3(light.direction_or_position));
	else
		lightDir = normalize(vec3(light.direction_or_position) - fragPos);
		
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)).rgb * light.diffuse;
	
	//specular
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = vec3(texture(material.specular, TexCoords)).rgb * spec * light.specular;

	//attenuation
	float dist = length(vec3(light.direction_or_position) - fragPos);
	float attenuation = 1.0;
	
	if(light.direction_or_position.w != 0.0)
		attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * sq(dist));
	
	ambient  *= attenuation; 
	diffuse  *= attenuation;
	specular *= attenuation; 	
	
    FragColor = vec4((ambient + diffuse + specular), 1.0);
}

float sq(float val)
{
	return val * val;
}


