#version 300 es

const float KERNEL_SIZE_FLOAT=25.0;

uniform mat4 uModelViewProjectionMatrix;

uniform vec2 uImageIncrement;

layout(location = 0) in vec3  position;
layout(location = 1) in vec3  normal;
layout(location = 2) in vec2  uv;

out vec2 vUv;



void main() {
    vUv = uv - ((KERNEL_SIZE_FLOAT-1.0)/2.0) * uImageIncrement;
    gl_Position = uModelViewProjectionMatrix * vec4(position, 1.0);
}
