#version 300 es

layout(location = 0) in vec3 VertexPosition;

uniform mat4 uMVPMatrix;


void main()
{
    gl_Position = uMVPMatrix * vec4(VertexPosition,1.0);
}
