#version 330 core

layout(location = 0) in vec3 aPos;

out vec2 vTex;
out vec3 vWorldPos;

uniform mat4 mvp;
uniform vec2 uvs[24];

void main() {
    vTex = uvs[gl_VertexID];
    vWorldPos = aPos;

    gl_Position = mvp * vec4(aPos, 1.0);
}
