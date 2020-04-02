#version 300 es

uniform mat4 uModelViewProjectionMatrix;

layout(location = 0) in vec3  position;
layout(location = 1) in vec3  normal;
layout(location = 2) in vec2  uv;

out vec2 vUv;



void main() {
    vUv = uv;
    gl_Position = uModelViewProjectionMatrix * vec4(position, 1.0);
}
