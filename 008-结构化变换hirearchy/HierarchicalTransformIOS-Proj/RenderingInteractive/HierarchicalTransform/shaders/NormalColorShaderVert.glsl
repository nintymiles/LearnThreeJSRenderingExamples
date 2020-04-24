#version 300 es

uniform mat4    uModelViewMatrix;
uniform mat4    uProjectionMatrix;


layout(location = 0) in vec3  position;
layout(location = 1) in vec3  normal;
layout(location = 2) in vec2  uv;

out vec3 vNormal;

void main() {
    vNormal = normal;
    gl_Position = uProjectionMatrix * uModelViewMatrix * vec4(position, 1.0);
}
