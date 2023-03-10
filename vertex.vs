#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec2 aTexCoord;

//out vec2 TexCoord;
flat out int predator;

uniform int predators[200];
uniform mat4 position[200];
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * position[gl_InstanceID] * vec4(aPos, 1.0f);
	//TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	predator = predators[gl_InstanceID];
}