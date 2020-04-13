//
//  Box.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Box_h
#define Box_h

#include <stdio.h>
#include <cmath>
#include <limits>

#include <vector>

#include "cvec.h"


using std::vector;
typedef Cvec3 vec3;

class Box {
    
public:
    vec3 min;
    vec3 max;
    
    Box(){}
    ~Box();
    
    void makeEmpty(){
        double infinity= std::numeric_limits<double>::infinity();
        min[0]=min[1]=min[2]=-infinity; //-INFINITY; //in math.h
        max[0]=max[1]=max[2]=infinity; //INFINITY; //in math.h
    }
    
    bool isEmpty(){
        return ( max[0] < min[0] ) || ( max[1] < min[1] ) || ( max[2] < min[2] );
    }
    
    void setFromPoints(vector<vec3> points){
        for(vec3 point:points){
            expandByPoint(point);
        }
    }
    
    void expandByPoint(vec3 point) {
        min=minVec(point,min);
        max=maxVec(point,max);
    }
    
    //获取的aabb box中心坐标的算法
    vec3 getCenter(){
        //如果box未初始化，返回vec3(0,0,0);
        if(isEmpty()) return vec3(0,0,0);
        return (min+max) * 0.5f;
    }


};

#endif /* Box_h */
