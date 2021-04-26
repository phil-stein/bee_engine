#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 dir;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    dir = aPos * -1;
    vec4 pos = proj * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
    // gl_Position = proj * view * vec4(aPos, 1.0);
}
