#version 300 es
precision highp float;

const int KERNEL_SIZE_INT = 25;

uniform float cKernel[KERNEL_SIZE_INT];

uniform sampler2D tDiffuse;
uniform vec2 uImageIncrement;

in vec2 vUv;

out vec4 fragColor;

void main() {
    
    vec2 imageCoord = vUv;
    vec4 sum = vec4( 0.0, 0.0, 0.0, 0.0 );
    
    for( int i = 0; i < KERNEL_SIZE_INT; i ++ ) {
        sum += texture(tDiffuse, imageCoord) * cKernel[i];
        imageCoord += uImageIncrement;
    }
    
    fragColor = sum;
    
}
