#version 330		

in vec4 vColor;
in vec2 texCoord;

out vec4 color;				

struct DirectionalLight 
{
	vec3 color;
	float ambientIntensity;
};

uniform sampler2D textureSampler;
uniform DirectionalLight directionalLight;

void main()					
{
	vec4 ambientColor = vec4(directionalLight.color, 1.0) * directionalLight.ambientIntensity;

	color = texture(textureSampler, texCoord) * ambientColor;
}
