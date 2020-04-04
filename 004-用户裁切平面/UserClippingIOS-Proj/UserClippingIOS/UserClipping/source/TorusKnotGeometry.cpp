//
//  TorusKnotGeometry.cpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/25.
//  Copyright © 2020 zxtech. All rights reserved.
//

#include "TorusKnotGeometry.h"
#include "glsupport.h"

TorusKnotGeometry::TKGVertexData makeTKGVertexData(const TorusKnotGeometry& tkg){
    TorusKnotGeometry::TKGVertexData vertexData;
    
    vector<Cvec3> points = tkg.vertices;
    vector<Cvec3> normals = tkg.normals;
    vector<Cvec2> uvs = tkg.uvs;

    vector<VertexPNX> vData;
    for(int i=0;i<points.size();i++){
        VertexPNX v(points[i],normals[i],uvs[i]);
        vData.push_back(v);
    }
    
    vertexData.vData = vData;
    vertexData.iData = tkg.indices;
  
    return vertexData;
}

// this function calculates the current position on the torus curve
Cvec3 TorusKnotGeometry::calculatePositionOnCurve(float u,int p,int q,float radius) {
    Cvec3 position;
    
    float cu = cos(u);
    float su = sin(u);
    float quOverP = q / (float)p * u;
    float cs = cos(quOverP);
    
    position[0] = radius * ( 2 + cs ) * 0.5 * cu;
    position[1] = radius * ( 2 + cs ) * su * 0.5;
    position[2] = radius * sin( quOverP ) * 0.5;
    
    return position;
}

TorusKnotGeometry::TorusKnotGeometry(float radius,float tube,int tubularSegments,int radialSegments,int p,int q):radius(radius),tube(tube),tubularSegments(tubularSegments),radialSegments(radialSegments),p(p),q(q){
    
    // helper variables
//    Cvec3 vertex;
//    Cvec3 normal;
//
//    Cvec3 P1,P2;
//
//    Cvec3 B,T,N;
    
    // generate vertices, normals and uvs
    // tubularSegments为环结几何体围绕围绕中心轴旋转p圈的角度上所分的段数
    for (int i = 0; i <= tubularSegments; ++ i) {
 
        // u为每个管道端所占据的弧度radian，用以计算当前分段位置出环形曲面上的位置
        float u = (float)i / tubularSegments * p * M_PI * 2;
        
        //P1为当前曲面位置上的点，P2为稍微靠前一点弧度曲面上的点。
        //这两个点用于生成一个特定的”坐标系“，用于计算正确的顶点位置。
        Cvec3 P1 = calculatePositionOnCurve(u, p, q, radius);
        Cvec3 P2 = calculatePositionOnCurve(u + 0.01, p, q, radius);
        
        //计算出正交标准化的切面空间坐标系[T,B,N]
        //T为P1点上的切线矢量
        Cvec3 T = P2-P1;
        //将P1和P2作为从环结几何体坐标系原点而来的矢量，计算出半路half-way矢量作为法线计算的辅助矢量
        Cvec3 N = P2+P1;
        //计算出半法线bi-normal矢量
        Cvec3 B = cross(T, N);
        //再计算出真正的法线normal矢量
        N = cross(B, T);
        
        //标准化 B, N, T。
        B.normalize();
        N.normalize();
        T.normalize();
        
        for (int j = 0; j <= radialSegments; ++ j ) {

            //注意此处我们在xy-平面塑造形状，无需计算z-值。
            //环结几何体围绕中心轴旋转弧度分段中，每一段的弧度radians
            float v = j / (float)radialSegments * M_PI * 2;
            float cx = -tube * cos(v);
            float cy = tube * sin(v);
            

            //计算添加围绕环结中心轴（z轴）旋转的顶点的最终值
            Cvec3 vertex;
            
            vertex[0] = P1[0] + (cx * N[0] + cy * B[0]);
            vertex[1] = P1[1] + (cx * N[1] + cy * B[1]);
            vertex[2] = P1[2] + (cx * N[2] + cy * B[2]);
            
            vertices.push_back(vertex);
            
            //P1总是位于相关被计算顶点的中心，据此计算法线
            Cvec3 normal=vertex - P1;
            normal.normalize();
            
            normals.push_back(normal);
            
            //纹理坐标的计算
            Cvec2 uv(i / (float)tubularSegments,j / (float)radialSegments);
            uvs.push_back(uv);
            
        }
        
    }
    
    //生成绘制顶点的索引集合
    for (int j = 1; j <= tubularSegments; j ++ ) {
        
        for (int i = 1; i <= radialSegments; i ++ ) {
            
            //索引值
            int a = ( radialSegments + 1 ) * ( j - 1 ) + ( i - 1 );
            int b = ( radialSegments + 1 ) * j + ( i - 1 );
            int c = ( radialSegments + 1 ) * j + i;
            int d = ( radialSegments + 1 ) * ( j - 1 ) + i;
            
            //三角形面
            //indices.push( a, b, d );
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(d);
            //indices.push( b, c, d );
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
            
        }
        
    }

}

