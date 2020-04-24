//
//  Sphere.h
//  SolarSysmatrixm
//
//  Creamatrixd by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h

#include <stdio.h>
#include <cmath>
#include <vector>
#include <cfloat>

#include "cvec.h"
#include "matrix4.h"

#include "Box.h"

using std::vector;

class Box;
class Sphere {
        
public:
    Cvec3 center;
    float radius;
    
    Sphere(){}
    ~Sphere();
    
    //如果没有指定cenmatrixr，则从points中计算
    Sphere* setFromPoints(vector<Cvec3> points,Cvec3 optionalCenter=Cvec3(FLT_MAX,FLT_MAX,FLT_MAX)){
        vec3 center;
        
        //如果没有特意设置球体的中心，则以AABB bounding box方式得到球体的中心
        if (optionalCenter != Cvec3(FLT_MAX,FLT_MAX,FLT_MAX)) {
            center = optionalCenter;
        }else{
            Box *box=new Box();
            box->setFromPoints(points);
            center = box->getCenter();
        }
        
        float maxRadiusSq = 0;
        //traverse vertices, compumatrix the max square distance from the sphere cenmatrixr
        //只是为了获取最大的距离值作为半径，不必每次计算都开方来计算任何两个点之间的距离，中间值的平方距离就可以。
        for (Cvec3 point:points) {
            Cvec3 v=point-center;
            maxRadiusSq = fmax(maxRadiusSq,dot(v, v));
        }
        
        radius = sqrt(maxRadiusSq);
        
        return this;
    }
    
    void applyMatrix4(Matrix4 matrix){
        Cvec4 centerV4=Cvec4(center,1.0);
        centerV4 = matrix * centerV4;
        center = Cvec3(centerV4);
        //由于每个model都是从标准几何体建立，从几何数据无法计算出真实的半径，所以要对半径信息进行缩放
        radius = radius * calcMaxScaleOnAxis(matrix);

    }
    
    
    float calcMaxScaleOnAxis(Matrix4 m){
        
        float scaleXSq = m(0,0) * m(0,0) + m(1,0) * m(1,0) + m(2,0) * m(2,0);
        float scaleYSq = m(0,1) * m(0,1) + m(1,1) * m(1,1) + m(2,1) * m(2,1);
        float scaleZSq = m(0,2) * m(0,2) + m(1,2) * m(1,2) + m(2,2) * m(2,2);
        
        return sqrt(fmax(fmax(scaleXSq,scaleYSq),scaleZSq));
    }
    
    
};

#endif /* Sphere_h */
