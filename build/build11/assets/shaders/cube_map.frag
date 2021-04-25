#version 460 core
	
out vec4 FragColor;
	
in vec3 dir;
uniform samplerCube cube_map;

void main()
{
	FragColor = texture(cube_map, dir);
}
