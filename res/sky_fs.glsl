#version 450 core

out vec4 FragColor;
in vec2 vUV;

uniform mat4 invView;

const vec3 topColor = vec3(.4, .6, 1.);
const vec3 horizonColor = vec3(.8, .9, 1.);
const vec3 bottomColor = vec3(.05, .1, .2);

const float FOV = radians(70.0);

void main() {
    vec2 ndc = vUV * 2.0 - 1.0;

    float tanHalfFov = tan(FOV * 0.5);

    vec3 viewDir = normalize(vec3(ndc.x * tanHalfFov, ndc.y * tanHalfFov, -1.0));

    vec3 worldDir = normalize((invView * vec4(viewDir, 0.0)).xyz);

    float y = worldDir.y;
    vec3 skyColor;

    if(y >= 0.0) {
        float t = pow(y, 0.75);
        skyColor = mix(horizonColor, topColor, t);
    } else {
        float t = pow(-y, 0.75);
        skyColor = mix(horizonColor, bottomColor, t);
    }

    FragColor = vec4(skyColor, 1.0);
}
