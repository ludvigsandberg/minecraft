#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in float aShadow;
layout(location = 3) in float aLight;

out vec2 vTex;
out vec3 vWorldPos;
out float vShadow;
out float vLight;

uniform mat4 mvp;

void main() {
    vTex = aTex;
    vWorldPos = aPos;
    vShadow = aShadow;
    vLight = aLight;

    gl_Position = mvp * vec4(aPos, 1.0);
}
