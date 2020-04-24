#version 300 es
precision highp float;

in vec3 vNormal;
out vec4 fragColor;

vec3 packNormal2Color(in vec3 normal) {
    return normalize(normal) * 0.5 + 0.5;
}

void main() {
    vec3 normalColor = packNormal2Color(vNormal);

    fragColor = vec4(normalColor,0.9);
}
