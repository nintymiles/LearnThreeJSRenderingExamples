#version 300 es
precision highp float;

uniform sampler2D tDiffuse;
uniform sampler2D effectTex;

in vec2 vUv;

out vec4 fragColor;

void main() {
    
    vec2 imageCoord = vUv;
    vec4 sum = vec4( 0.0, 0.0, 0.0, 0.0 );
    
    sum += texture(tDiffuse, imageCoord);
    sum += texture(effectTex, imageCoord);
    
    fragColor = sum;
//    fragColor = texture(tDiffuse, imageCoord);
    
}
