#version 330		

in vec4 vColor;
in vec2 texCoord;
in vec3 Normal;

out vec4 color;				

struct DirectionalLight 
{
	vec3 color;
	float ambientIntensity;
	vec3 direction;
	float diffuseIntensity;
};

uniform sampler2D textureSampler;
uniform DirectionalLight directionalLight;

void main()					
{
	vec4 ambientColor = vec4(directionalLight.color, 1.0) * directionalLight.ambientIntensity;
	float diffuseFactor = max(dot(normalize(Normal), normalize(directionalLight.direction)), 0.0);
	vec4 diffuseColor = vec4(directionalLight.color, 1.0) * directionalLight.diffuseIntensity * diffuseFactor;
	color = texture(textureSampler, texCoord) * (ambientColor + diffuseColor);
}
