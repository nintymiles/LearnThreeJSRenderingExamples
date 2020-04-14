#ifndef Ray_h
#define Ray_h

#include "glsupport.h"

#include "cvec.h"
#include "matrix4.h"

using namespace std;

class Sphere;
class Box;

typedef Cvec3 vec3;
typedef Cvec4 vec4;
typedef Matrix4 mat4;


class Ray{
public:
    vec3 origin;
    vec3 direction;// destination, dirrection
    
    Ray();
    Ray(vec3 de, vec3 di);
    Ray(const Ray & r);
    ~Ray();
    
    Ray & operator = (const Ray & r);
    void printInformation() const;
    
    //根据射线属性，将射线方向乘以长度标量t，再加上相机位置，就获得相交点的位置
    vec3 at(float t);
    
    //ray-sphere相交的基本计算
    vector<vec3> intersectSphere(Sphere* sphere);
    //ray-box相交
    vector<vec3> intersectBox(Box* box);
    //ray-triangle
    vector<vec3> intersectTriangle(vec3 a,vec3 b,vec3 c,bool backfaceculling);
    
    void applyMatrix4(mat4 matrix){
        vec4 originV4=vec4(origin,1.0);
        origin = vec3(matrix * originV4);
        
        vec4 directionV4 = vec4(direction,0.0);
        direction = vec3(matrix * directionV4);
        direction.normalize();
    }
};

#endif
