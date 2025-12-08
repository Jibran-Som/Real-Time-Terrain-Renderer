#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float brightness = 1.0;

void main() {
    vec3 color = texture(skybox, TexCoords).rgb;
    FragColor = vec4(color * brightness, 1.0);
}