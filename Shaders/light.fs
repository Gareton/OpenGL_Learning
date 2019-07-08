#version 430 core
#define NR_POINT_LIGHTS 4

struct Material{
    sampler2D diffuse;
    sampler2D specular;
	sampler2D emission;
    float shininess;
};

struct DirLight
{
	vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
	vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float constant;
	float linear;
	float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float innerCone;
	float outerCone;
	
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
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;

float sq(float val);
vec3 calcDirLight(DirLight l, vec3 normal, vec3 viewDir);
vec3 calcPointLight(PointLight l, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 calcSpotLight(SpotLight l, vec3 normal, vec3 viewDir, vec3 fragPos);

void main()
{	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 resColor = vec3(0.0, 0.0, 0.0);
	
	resColor += calcDirLight(dirLight, norm, viewDir);
	resColor += calcSpotLight(spotLight, norm, viewDir, fragPos);
	
	for(int i = 0; i < NR_POINT_LIGHTS; ++i)
		resColor += calcPointLight(pointLights[i], norm, viewDir, fragPos);

    FragColor = vec4(resColor, 1.0);
}

vec3 calcDirLight(DirLight l, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-l.direction);
	vec3 reflectDir = reflect(-lightDir, normal);
	
	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);


	vec3 ambient = vec3(texture(material.diffuse, TexCoords)) * l.ambient;	
	vec3 diffuse = diff * vec3(texture(material.diffuse, TexCoords)) * l.diffuse;
	vec3 specular = vec3(texture(material.specular, TexCoords)) * spec * l.specular;	
	
    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight l, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 lightDir = normalize(l.position - fragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
 
    float diff = max(dot(normal, lightDir), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(l.position - fragPos);
    float attenuation = 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));    

    vec3 ambient = l.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = l.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = l.specular * spec * vec3(texture(material.specular, TexCoords));
	
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
	return (ambient + diffuse + specular);
}

vec3 calcSpotLight(SpotLight l, vec3 normal, vec3 viewDir, vec3 fragPos)
{
	vec3 lightDir = normalize(l.position - fragPos);
	vec3 ambient = l.ambient * vec3(texture(material.diffuse, TexCoords));

	float theta = dot(lightDir, normalize(-l.direction));
	
	vec3 reflectDir = reflect(-lightDir, normal);
 
	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	float distance = length(l.position - fragPos);
	float attenuation = 1.0 / (l.constant + l.linear * distance + l.quadratic * (distance * distance));    

	vec3 diffuse = l.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = l.specular * spec * vec3(texture(material.specular, TexCoords));
		
	float epsilon = l.innerCone - l.outerCone;
	float intensity = clamp((theta - l.outerCone) / epsilon, 0.0, 1.0);
		
	ambient *= attenuation;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;
	
	return (ambient + diffuse + specular);

}

float sq(float val)
{
	return val * val;
}


