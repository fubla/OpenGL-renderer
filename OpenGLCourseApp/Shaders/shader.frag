#version 330		

in vec4 vColor;
in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;
in vec4 DirectionalLightSpacePos;

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
uniform sampler2D directionalShadowMap;

uniform Material material;

uniform vec3 eyePos;

float CalcDirectionalShadowFactor(DirectionalLight light)
{
	vec3 projCoords = DirectionalLightSpacePos.xyz / DirectionalLightSpacePos.w;
	projCoords = (projCoords * 0.5) + 0.5;

	float current = projCoords.z;

	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.direction);

	float bias = max(0.05 * (1 - dot(normal, lightDirection)), 0.005);

	float shadow = 0.0;

	vec2 texSize = 1.0 / textureSize(directionalShadowMap, 0);
	for (int x = -1; x <= 1; x++)
	{
		for(int y = -1; y <= 1; y++)
		{
			float pdfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texSize).r;
			shadow += current - bias > pdfDepth ? 1.0 : 0.0;
		}
	}

	shadow /= 9.0;

	if(projCoords.z > 1.0){
		shadow = 0.0;	
	}
	
	return shadow;
}

vec4 CalcLightByDirection(Light light, vec3 direction, float shadowFactor) 
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

	return ambientColor + (1.0 - shadowFactor) * (diffuseColor + specularColor);
}

vec4 CalcDirectionalLight()
{
	float shadowFactor = CalcDirectionalShadowFactor(directionalLight);
	return CalcLightByDirection(directionalLight.base, directionalLight.direction, shadowFactor);
}

vec4 CalcPointLight(PointLight pLight)
{
	vec3 direction = FragPos - pLight.position;
	float dist = length(direction);
	direction = normalize(direction);

	vec4 color = CalcLightByDirection(pLight.base, direction, 0.0);
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
