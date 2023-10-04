#version 330		

in vec4 vColor;
in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;		

const int MAX_POINT_LIGHTS = 3;

struct Light
{
	vec3 color;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight 
{
	Light base;
	vec3 direction;
};

struct PointLight
{
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D textureSampler;
uniform Material material;

uniform vec3 eyePos;

vec4 CalcLightByDirection(Light light, vec3 direction) 
{
	vec4 ambientColor = vec4(light.color, 1.0) * light.ambientIntensity;
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0);
	vec4 diffuseColor = vec4(light.color, 1.0) * light.diffuseIntensity * diffuseFactor;
	vec4 specularColor = vec4(0, 0, 0, 0);
	bool isLit = diffuseFactor > 0.0;
	vec3 fragToEye = normalize(eyePos - FragPos);
	vec3 refl = normalize(reflect(direction, normalize(Normal)));
	float specularFactor = max(dot(fragToEye, refl), 0.0);
	specularFactor = pow(specularFactor, material.shininess);
	specularColor = vec4(light.color, 1.0) * material.specularIntensity * specularFactor * float(isLit);

	return ambientColor + diffuseColor + specularColor;
}

vec4 CalcDirectionalLight()
{
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++)
	{
		vec3 direction = FragPos - pointLights[i].position;
		float dist = length(direction);
		direction = normalize(direction);

		vec4 color = CalcLightByDirection(pointLights[i].base, direction);
		float attenuation = pointLights[i].exponent * dist * dist + 
							pointLights[i].linear * dist +
							pointLights[i].constant;

		totalColor += color / attenuation;
	}
	return totalColor;

}

void main()					
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	
	color = texture(textureSampler, texCoord) * finalColor;
}
