#version 450 core

in vec2 vTex;
in vec3 vWorldPos;
in float vShadow;
in float vLight;

out vec4 FragColor;

uniform sampler2D atlas;
uniform vec3 cameraPos;

const vec3 topColor = vec3(.4, .6, 1.);
const vec3 horizonColor = vec3(.8, .9, 1.);
const vec3 bottomColor = vec3(.05, .1, .2);

const float fogStart = 32.;
const float fogEnd = 96.;

void main() {
    vec4 baseColor = texture(atlas, vTex);

    vec3 toFrag = vWorldPos - cameraPos;
    float dist = length(toFrag);
    vec3 viewDir = normalize(toFrag);

    float fogFactor = clamp((fogEnd - dist) / (fogEnd - fogStart), 0.0, 1.0);

    float y = viewDir.y;
    vec3 fogColor;

    if(y >= 0.0) {
        float t = pow(y, 0.75);
        fogColor = mix(horizonColor, topColor, t);
    } else {
        float t = pow(-y, 0.75);
        fogColor = mix(horizonColor, bottomColor, t);
    }

    vec3 color = mix(fogColor, baseColor.rgb * vShadow * (vLight / 15.0), fogFactor);
    FragColor = vec4(color, baseColor.a);
}
