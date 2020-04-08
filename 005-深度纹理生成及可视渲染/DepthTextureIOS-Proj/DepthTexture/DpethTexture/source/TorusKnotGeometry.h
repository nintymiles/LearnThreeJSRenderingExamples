//
//  FrustumGeometry.hpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/25.
//  Copyright © 2020 zxtech. All rights reserved.
//

#ifndef TorusKnotGeometry_H
#define TorusKnotGeometry_H

#include <stdio.h>
#include <map>
#include <vector>

#include "cvec.h"
#include "Geometry.h"

#include "matrix4.h"

using namespace std;

class TorusKnotGeometry{
    float radius;
    float tube;
    int tubularSegments;
    int radialSegments;
    float p,q;
    
    vector<Cvec3> vertices;
    vector<Cvec3> normals;
    vector<Cvec2> uvs;
    vector<unsigned short> indices;
    
    Cvec3 calculatePositionOnCurve(float u,int p,int q,float radius);
    
public:
    /**
     * class TorusKnotGeometry constructor
     * @param radius 整个torus knot环形结几何体的半径
     * @param tube   环形结轮廓管道半径
     * @param tubularSegments 管道的数据段数
     * @param radialSegments  环形几何体横截面的数据段数
     * @param p 环形几何体围绕中心轴扭转的圈数
     * @param q 环形几何体内部圆环的数量
     *
     */
    TorusKnotGeometry(float radius=1.0,float tube=0.3,int tubularSegments=64,int radialSegments=8,int p=2,int q=3);
    ~TorusKnotGeometry(){};
    
    struct TKGVertexData{
        vector<VertexPNX> vData;
        vector<unsigned short> iData;
    };
    
    
    friend TKGVertexData makeTKGVertexData(const TorusKnotGeometry&);
};

TorusKnotGeometry::TKGVertexData makeTKGVertexData(const TorusKnotGeometry&);

#endif /* TorusKnotGeometry_H */
