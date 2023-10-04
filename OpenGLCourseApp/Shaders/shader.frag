#version 330		

in vec4 vColor;
in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;		

const int MAX_POINT_LIGHTS = 3;
const int MAX_SPOT_LIGHTS = 3;

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

struct SpotLight
{
	PointLight base;
	vec3 direction;
	float edge;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;
uniform int spotLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

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

vec4 CalcPointLight(PointLight pLight)
{
	vec3 direction = FragPos - pLight.position;
	float dist = length(direction);
	direction = normalize(direction);

	vec4 color = CalcLightByDirection(pLight.base, direction);
	float attenuation = pLight.exponent * dist * dist + 
						pLight.linear * dist +
						pLight.constant;

	return color / attenuation;
}

vec4 CalcSpotLight(SpotLight sLight)
{
	vec3 rayDirection = normalize(FragPos - sLight.base.position);
	float spotLightFactor = dot(rayDirection, sLight.direction);

	if(spotLightFactor > sLight.edge) 
	{
		vec4 color = CalcPointLight(sLight.base);

		return color * (1.0 - (1.0 - spotLightFactor) * (1.0 / (1.0 - sLight.edge)));
	} else {
		return vec4(0, 0, 0, 0);
	}
}

vec4 CalcPointLights()
{
	vec4 totalColor = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++)
	{
		totalColor += CalcPointLight(pointLights[i]);
	}
	return totalColor;
}

vec4 CalcSpotLights()
{
	vec4 totalColor = vec4(0, 0, 0, 0);
	for(int i = 0; i < spotLightCount; i++)
	{
		totalColor += CalcSpotLight(spotLights[i]);
	}
	return totalColor;
}

void main()					
{
	vec4 finalColor = CalcDirectionalLight();
	finalColor += CalcPointLights();
	finalColor += CalcSpotLights();
	
	color = texture(textureSampler, texCoord) * finalColor;
}
