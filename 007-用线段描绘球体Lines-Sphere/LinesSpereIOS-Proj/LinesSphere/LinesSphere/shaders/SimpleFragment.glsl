#version 300 es
precision highp float;

uniform vec3 uColor;
uniform float uAlpha;


out vec4 FragColor;

void main() {
    FragColor = vec4(uColor,uAlpha);
}
