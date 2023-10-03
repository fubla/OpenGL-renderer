#version 330		

in vec4 vColor;
in vec2 texCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 color;				

struct DirectionalLight 
{
	vec3 color;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

struct Material
{
	float specularIntensity;
	float shininess;
};

uniform sampler2D textureSampler;
uniform DirectionalLight directionalLight;
uniform Material material;

uniform vec3 eyePos;

void main()					
{
	vec4 ambientColor = vec4(directionalLight.color, 1.0) * directionalLight.ambientIntensity;
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0);
	vec4 diffuseColor = vec4(directionalLight.color, 1.0) * directionalLight.diffuseIntensity * diffuseFactor;
	vec4 specularColor = vec4(0, 0, 0, 0);
	bool isLit = diffuseFactor > 0.0;
	vec3 fragToEye = normalize(eyePos - FragPos);
	vec3 refl = normalize(reflect(directionalLight.direction, normalize(Normal)));
	float specularFactor = max(dot(fragToEye, refl), 0.0);
	specularFactor = pow(specularFactor, material.shininess);
	specularColor = vec4(directionalLight.color, 1.0) * material.specularIntensity * specularFactor * float(isLit);

	color = texture(textureSampler, texCoord) * (ambientColor + diffuseColor + specularColor);
}
