#version 450 core

in vec2 vUV;

out vec4 FragColor;

uniform sampler2D charset;

void main() {
    FragColor = texture(charset, vUV);
}
