//
//  CubeGeometry.hpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/4/8.
//  Copyright © 2020 zxtech. All rights reserved.
//

#ifndef CubeGeometry_H
#define CubeGeometry_H

#include <stdio.h>
#include <map>
#include <vector>

#include "cvec.h"
#include "Geometry.h"

#include "matrix4.h"

using namespace std;

struct IndexGroup{
    int groupId;
    int groupStart;
    int groupCount;
    
    IndexGroup(int groupId,int groupStart,int groupCount):groupId(groupId),groupStart(groupStart),groupCount(groupCount){};
};

class CubeGeometry{
    float width, height, depth;
    int widthSegments, heightSegments, depthSegments;
    
    // helper variables
    int numberOfVertices = 0;
    int groupStart = 0;
    
    vector<Cvec3> vertices;
    vector<Cvec3> normals;
    vector<Cvec2> uvs;
    vector<unsigned short> indices;
    
    vector<IndexGroup> groups;
    
    void buildPlane(int id1,int id2,int id3,int udir,int vdir,float width,float height,float depth,int gridX,int gridY,int materialIndex);
    
public:
    
    /**
     * class CubeGeometry constructor
     * @param width     宽度
     * @param height    高度
     * @param depth     深度
     * @param widthSegments  宽度分段
     * @param heightSegments 高度分段
     * @param depthSegments  深度分段
     *
     */
    CubeGeometry(float width=1,float height=1,float depth=1,int widthSegments=1,int heightSegments=1,int depthSegments=1);
    
    ~CubeGeometry(){};
    
    struct CubeVertexData{
        vector<VertexPNX> vData;
        vector<unsigned short> iData;
    };
    
    
    friend CubeVertexData makeCubeVertexData(const CubeGeometry&);
};

CubeGeometry::CubeVertexData makeCubeVertexData(const CubeGeometry&);

#endif /* CubeGeometry_H */
