#version 300 es
precision highp float;

//--------------------------------------------------------------------------------
//  uniform variables
//--------------------------------------------------------------------------------
// uniform variable -- material specular color
uniform vec4      vMaterialSpecular;
//uniform  vec4      vMaterialDiffuse;
uniform vec3       vMaterialAmbient;
// uniform variable -- light position
uniform vec3  vLight0;

//--------------------------------------------------------------------------------
//  fragment input variables -- varying varialbles
//--------------------------------------------------------------------------------
//varying variables -- diff use calculated in vertex shader
in vec4 colorDiffuse;
// varying variables -- vertex position in world frame
in vec3 position;
// varying variables -- normal vector in world frame
in vec3 normal;
// varying variables -- texture coordinate
in vec2 texCoord;

//in float vDistance;

//--------------------------------------------------------------------------------
//  fragment output varaible declaration
//--------------------------------------------------------------------------------
// the outputing fragment color
out vec4 fragColor;

void main(){
    //if(vDistance<0.0)
        //discard;
    
    vec3 halfVector = normalize(vLight0 - position);
    float NdotH = max(dot(normalize(normal), halfVector), 0.0);
    float fPower = vMaterialSpecular.w;
    float specular = pow(NdotH, fPower);
    
    vec4 colorSpecular = vec4( vMaterialSpecular.xyz * specular, 1 );
    fragColor = colorDiffuse + colorSpecular + vec4(vMaterialAmbient,1);
    //fragColor = vMaterialDiffuse * NdotH + colorSpecular;
    
    fragColor =  colorDiffuse +  vec4(vMaterialAmbient.xyz, 1.0f) + colorSpecular;
        
}
