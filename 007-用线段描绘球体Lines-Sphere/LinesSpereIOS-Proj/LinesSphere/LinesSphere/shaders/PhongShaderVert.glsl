//OpenGL version specification
#version 300 es

//--------------------------------------------------------------------------------
// 统一参数变量统一设置
//--------------------------------------------------------------------------------
//uniform变量 -- 4X4模型视图矩阵
uniform  mat4      uModelViewMatrix;
//unifrom变量 -- 4X4投射矩阵(projection)
uniform  mat4      uProjectionMatrix;

//unifrom变量 -- 光位置矢量,以中等精度表示
uniform  vec3  vLight0;

//uniform variable, store material diffuse color vector (4 components)
uniform  vec4       vMaterialDiffuse;
//uniform variable, store material ambient color vector (3 components)
uniform  vec3       vMaterialAmbient;
//uniform variable, store material specular color vector (4 components)
uniform  vec4       vMaterialSpecular;

uniform  vec4       uUserClipPlane;

//--------------------------------------------------------------------------------
// vertex inputs(attributes) variables
//--------------------------------------------------------------------------------
// vertex input variable 顶点位置vector
layout(location = 0) in vec3 myVertex;
// vertex input variable 法线vector
layout(location = 1) in vec3 myNormal;
// vertex input variable UV
layout(location = 2) in vec2 myUV;


//--------------------------------------------------------------------------------
//  vertex output variables aka. varying variables declaration
//--------------------------------------------------------------------------------
// varying variable 纹理坐标
out vec2    texCoord;
// varying variable 漫射颜色
out vec4    colorDiffuse;

// varying variable 顶点位置（以world frame表示）
out vec3    position;
// varying variable 法线（以world frame表示）
out vec3    normal;

//out float vDistance;


void main(void){
    //将顶点的对象坐标以4-coordinate vector赋给P
    vec4 p = vec4(myVertex,1);
    //转换顶点
    vec4 eyePos = uModelViewMatrix * p;
    
    
    
    texCoord = myUV;
    
    //将对象空间的normal vector变为eye frame空间的normal vector
    vec3 eyeNormal = vec3(mat3(uModelViewMatrix[0].xyz, uModelViewMatrix[1].xyz, uModelViewMatrix[2].xyz) * myNormal);
    //取出顶点坐标为3-coordinate vector方式,ecPosition为眼睛坐标
    vec3 ecPosition = eyePos.xyz;
    
//    //计算到user clip plane的距离
//    vDistance = dot(ecPosition,uUserClipPlane.xyz)+uUserClipPlane.w;
    
    //Diffuse factor的计算
    colorDiffuse = max(0.0,dot(normalize(eyeNormal), normalize(vLight0 - ecPosition))) * vMaterialDiffuse ;
    
    normal = eyeNormal;
    position = ecPosition;
    
    gl_Position = uProjectionMatrix * eyePos;
    
}
