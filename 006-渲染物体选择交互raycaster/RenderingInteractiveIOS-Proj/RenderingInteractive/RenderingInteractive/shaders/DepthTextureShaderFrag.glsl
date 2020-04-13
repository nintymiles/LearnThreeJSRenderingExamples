#version 300 es
precision highp float;

uniform sampler2D tDiffuse;
uniform sampler2D tDepth;
uniform float cameraNear;
uniform float cameraFar;

in vec2 vUv;
out vec4 fragColor;

float viewZToOrthographicDepth( const in float viewZ, const in float near, const in float far ) {
    return ( viewZ + near ) / ( near - far );
    //return (viewZ-(near+far)/2.0)/(far-near)+1.0/2.0;
}

float perspectiveDepthToViewZ( const in float invClipZ, const in float near, const in float far ) {
    return ( near * far ) / ( ( far - near ) * invClipZ - far );
    //return ((2.0*invClipZ-1.0)*(far-near)+2.0*(far*near))/(far+near);
}

float readDepth( sampler2D depthSampler, vec2 coord ) {
    float fragCoordZ = texture(depthSampler, coord).x;
    float viewZ = perspectiveDepthToViewZ(fragCoordZ, cameraNear, cameraFar);
    return viewZToOrthographicDepth( viewZ, cameraNear, cameraFar );
}

void main() {
    //vec3 diffuse = texture2D( tDiffuse, vUv ).rgb;
    float depth = readDepth(tDepth, vUv);
    
    vec4 depthColor = texture(tDepth, vUv) ;
    fragColor = texture(tDiffuse, vUv);
    fragColor = vec4(depthColor.r * 10.0);
    fragColor.a = 1.0;
    

//    fragColor.rgb = 1.0 - vec3( depth );
//    fragColor.a = 1.0;
    
//    vec4 outColor = vec4(vec3(texture(tDepth, vUv).r), 1.0);
//    // Make the image appear darker to make it clearly visible.
//    fragColor = pow( outColor, vec4(10.0) );
}

