#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in float aShadow;

out vec2 vTex;
out vec3 vWorldPos;
out float vShadow;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vTex = aTex;

    vec4 worldPos = model * vec4(aPos, 1.0);

    vWorldPos = worldPos.xyz;

    vShadow = aShadow;

    gl_Position = projection * view * worldPos;
}
