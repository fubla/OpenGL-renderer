#version 330

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 directionalLightTransform; // projection * view, from the point of view of the light source

void main()
{
	gl_Position = directionalLightTransform * model * vec4(pos, 1.0);
}