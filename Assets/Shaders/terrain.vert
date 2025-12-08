#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoords;
out vec3 WorldPos;
out float Height;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 world = model * vec4(aPos, 1.0);

    WorldPos = world.xyz;
    TexCoords = aTex;
    Height = world.y;

    gl_Position = projection * view * world;
}
