#version 300 es
precision highp float;

uniform float time;

uniform float fogDensity;
uniform vec3 fogColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 vUv;

out vec4 fragColor;

void main( void ) {

    //纹理坐标域更改为canonical域
    vec2 position = - 1.0 + 2.0 * vUv;
    
    vec4 noise = texture(texture1, vUv);
    
    //生成向某个方向运动的变化坐标值
    vec2 T1 = vUv + vec2( 1.5, - 1.5 ) * time * 0.02;
    vec2 T2 = vUv + vec2( - 0.5, 2.0 ) * time * 0.01;


    T1.x += noise.x * 2.0;
    T1.y += noise.y * 2.0;
    T2.x -= noise.y * 0.2;
    T2.y += noise.z * 0.2;

    float p = texture(texture1, T1 * 2.0).a;

    vec4 color = texture(texture2, T2 * 2.0);
    vec4 temp = color * ( vec4( p, p, p, p ) * 2.0 ) + ( color * color - 0.1 );

    if( temp.r > 1.0 ) { temp.bg += clamp( temp.r - 2.0, 0.0, 100.0 ); }
    if( temp.g > 1.0 ) { temp.rb += temp.g - 1.0; }
    if( temp.b > 1.0 ) { temp.rg += temp.b - 1.0; }

    fragColor = temp;

    float depth = gl_FragCoord.z;
    const float LOG2 = 1.442695;
    float fogFactor = exp2( - fogDensity * fogDensity * depth * depth * LOG2 );
    fogFactor = 1.0 - clamp( fogFactor, 0.0, 1.0 );

    fragColor = mix( fragColor, vec4( fogColor, fragColor.w ), fogFactor );

    //fragColor += vec4(0.2,0.2,0.2,0.0);
}

