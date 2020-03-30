//
//  FrustumGeometry.cpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/3/25.
//  Copyright © 2020 zxtech. All rights reserved.
//

#include "FrustumGeometry.h"
#include "glsupport.h"

vector<VertexPN> makeFrustVertexPNData(const FrustumGeometry& fg){
    vector<VertexPN> vertexData;
    
    vector<Cvec3> points = fg.vertices;
    vector<Cvec3> colors = fg.colors;
    
    for(int i=0;i<points.size();i++){
        VertexPN v(points[i],colors[i]);
        vertexData.push_back(v);
    }
  
    return vertexData;
}

FrustumGeometry::FrustumGeometry(Matrix4 projMat):projMat(projMat){
    
    // 线段的色彩信息
    Cvec3 colorFrustum = hexStringToRGB("0xffaa00");
    Cvec3 colorCone = hexStringToRGB("0xff0000");
    Cvec3 colorUp = hexStringToRGB("0x00aaff");
    Cvec3 colorTarget = hexStringToRGB("0xffffff");
    Cvec3 colorCross = hexStringToRGB("0x333333");
    
    //注意：下面每段中每个点的名称，这些名称代表了frustum锥体每个顶点的名称
    
    // 近平面方形的四条线段和对应色彩
    addLine( "n1", "n2", colorFrustum );
    addLine( "n2", "n4", colorFrustum );
    addLine( "n4", "n3", colorFrustum );
    addLine( "n3", "n1", colorFrustum );
    
    // 远平面方形的四条线段和对应色彩
    addLine( "f1", "f2", colorFrustum );
    addLine( "f2", "f4", colorFrustum );
    addLine( "f4", "f3", colorFrustum );
    addLine( "f3", "f1", colorFrustum );
    
    // 从近平面到远平面锥体边缘线段和色彩
    addLine( "n1", "f1", colorFrustum );
    addLine( "n2", "f2", colorFrustum );
    addLine( "n3", "f3", colorFrustum );
    addLine( "n4", "f4", colorFrustum );
    
    // 从眼睛坐标原点到近平面的锥体顶端线段和色彩
    addLine( "p", "n1", colorCone );
    addLine( "p", "n2", colorCone );
    addLine( "p", "n3", colorCone );
    addLine( "p", "n4", colorCone );
    
    // 指示上方方向的三角形线段和色彩
    addLine( "u1", "u2", colorUp );
    addLine( "u2", "u3", colorUp );
    addLine( "u3", "u1", colorUp );
    
    // 锥体中线线段和色彩
    addLine( "c", "t", colorTarget );
    addLine( "p", "c", colorCross );
    
    // 近平面和远平面中间轴对齐十字线线段和色彩
    addLine( "cn1", "cn2", colorCross );
    addLine( "cn3", "cn4", colorCross );
    
    addLine( "cf1", "cf2", colorCross );
    addLine( "cf3", "cf4", colorCross );
    
    update();
}

void FrustumGeometry::addLine(string a,string b,Cvec3 color){
    addPoint(a, color);
    addPoint(b, color);
    
}

void FrustumGeometry::addPoint(string id, Cvec3 color){
    //存储每个点的占位位置信息
    vertices.push_back(Cvec3(0,0,0));
    //存储每个点的色彩信息
    colors.push_back(color);
    
    //我们使用GL_LINES primtive类型绘制线段，每个线段由两个点确定，
    //根据我们的绘制方式，那么相对于锥体的每个真实顶点，这些线段会多次使用锥体的每个顶点。
    //所以，我们会记录每个顶点的使用详情，便于随后更新时根据顶点名称更新真实位置信息
    if (pointMap.find(id)==pointMap.end()){
        pointMap[id] = vector<int>();
    }
    pointMap[id].push_back((int)vertices.size() - 1);
}

void FrustumGeometry::setPoint(string p,float x,float y,float z){
    //先将ndc坐标反转投射为eye coordinate
    Cvec4 eyeP4 = inv(projMat) * Cvec4(x,y,z,1.0);
    //点的同质坐标需要转换为标准仿射坐标
    Cvec4 eyeP4Affine = eyeP4/eyeP4[3];
    Cvec3 eyePos = Cvec3(eyeP4Affine);
    
    //根据顶点名称，更新vertices中占位位置信息的真实值
    vector<int> points = pointMap[p];
    if (points.size()>0) {
        for (int i = 0, l = (int)points.size(); i < l; i ++ ) {
            vertices[points[i]] = eyePos;
        }
    }
    
}

void FrustumGeometry::update(){
    // the number for width/height settings
    int w = 1, h = 1;
    
    // we need just camera projection matrix
    // world matrix must be identity
    //camera.projectionMatrix.copy( this.camera.projectionMatrix );
    
    // z coords are 1 when siting on near plane and -1 when sitting on
    // far plane due to right handed ndc coordinates system
    
    // 近平面和远平面中心点
    setPoint( "c", 0, 0, 1 );
    setPoint( "t", 0, 0, -1 );
    
    // 近平面顶点
    setPoint( "n1", - w, - h, 1 );
    setPoint( "n2", w, - h, 1 );
    setPoint( "n3", - w, h, 1 );
    setPoint( "n4", w, h, 1 );
    
    // 远平面顶点
    setPoint( "f1", - w, - h, -1 );
    setPoint( "f2", w, - h, -1 );
    setPoint( "f3", - w, h, -1 );
    setPoint( "f4", w, h, -1 );
    
    // 指示向上方向的三角形顶点
    setPoint( "u1", w * 0.7, h * 1.1, 1 );
    setPoint( "u2", - w * 0.7, h * 1.1, 1 );
    setPoint( "u3", 0, h * 2, 1 );
    
    // 远平面十字相交线顶点
    setPoint( "cf1", - w, 0, -1 );
    setPoint( "cf2", w, 0, -1 );
    setPoint( "cf3", 0, - h, -1 );
    setPoint( "cf4", 0, h, -1 );
    
    // 近平面十字相交线顶点
    setPoint( "cn1", - w, 0, 1 );
    setPoint( "cn2", w, 0, 1 );
    setPoint( "cn3", 0, - h, 1 );
    setPoint( "cn4", 0, h, 1 );
        
};

void FrustumGeometry::updateWithProjMat(Matrix4 proj){
    this->projMat = proj;
    update();
}
