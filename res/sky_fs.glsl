#version 330 core

out vec4 FragColor;
in vec2 vUV;

uniform mat4 invProjection;
uniform mat4 invView;

const vec3 topColor = vec3(.4, .6, 1.);
const vec3 horizonColor = vec3(.8, .9, 1.);
const vec3 bottomColor = vec3(.05, .1, .2);

void main() {
    vec2 ndc = vUV * 2.0 - 1.0;
    vec4 clip = vec4(ndc, -1.0, 1.0);
    vec4 view = invProjection * clip;
    view /= view.w;
    vec4 world = invView * vec4(view.xyz, 0.0);
    vec3 viewDir = normalize(world.xyz);

    float y = viewDir.y;
    vec3 skyColor;

    if (y >= 0.0) {
        float t = pow(y, 0.75);
        skyColor = mix(horizonColor, topColor, t);
    } else {
        float t = pow(-y, 0.75);
        skyColor = mix(horizonColor, bottomColor, t);
    }

    FragColor = vec4(skyColor, 1.0);
}
