//
//  CubeGeometry.cpp
//  UtahTeapot
//
//  Created by SeanRen on 2020/4/8.
//  Copyright © 2020 zxtech. All rights reserved.
//

#include "CubeGeometry.h"
#include "glsupport.h"

CubeGeometry::CubeVertexData makeCubeVertexData(const CubeGeometry& cb){
    CubeGeometry::CubeVertexData vertexData;
    
    vector<Cvec3> points = cb.vertices;
    vector<Cvec3> normals = cb.normals;
    vector<Cvec2> uvs = cb.uvs;

    vector<VertexPNX> vData;
    for(int i=0;i<points.size();i++){
        VertexPNX v(points[i],normals[i],uvs[i]);
        vData.push_back(v);
    }
    
    vertexData.vData = vData;
    vertexData.iData = cb.indices;
  
    return vertexData;
}

// this function calculates the current position on the torus curve
void CubeGeometry::buildPlane(int idx1,int idx2,int idx3,int udir,int vdir,float width,float height,float depth,int gridX,int gridY,int materialIndex){
    float segmentWidth = width / gridX;
    float segmentHeight = height / gridY;
    
    float widthHalf = width / 2;
    float heightHalf = height / 2;
    float depthHalf = depth / 2;
    
    int gridX1 = gridX + 1;
    int gridY1 = gridY + 1;
    
    int vertexCounter = 0;
    int groupCount = 0;
    
    // generate vertices, normals and uvs
    
    for (int iy = 0; iy < gridY1; iy ++){
        
        float y = iy * segmentHeight - heightHalf;
        
        for (int ix = 0; ix < gridX1; ix ++) {
            
            float x = ix * segmentWidth - widthHalf;
            
            Cvec3 vertice;
            // set values to correct vector component
            vertice[idx1] = x * udir;
            vertice[idx2] = y * vdir;
            vertice[idx3] = depthHalf;
            
            // now apply vertice to vertex buffer
            vertices.push_back(vertice);
            
            Cvec3 normal;
            // set values to correct vector component
            normal[idx1] = 0;
            normal[idx2] = 0;
            normal[idx3] = depth > 0 ? 1 : - 1;
            
            normals.push_back(normal);
            
            // uvs
            uvs.push_back(Cvec2(ix/gridX,1-(iy/gridY)));
            
            // counters
            vertexCounter += 1;
        }
        
    }
    
    // indices
    // 1. you need three indices to draw a single face
    // 2. a single segment consists of two faces
    // 3. so we need to generate six (2*3) indices per segment
    
    for (int iy = 0; iy < gridY; iy ++ ) {
        
        for (int ix = 0; ix < gridX; ix ++ ) {
            
            int a = numberOfVertices + ix + gridX1 * iy;
            int b = numberOfVertices + ix + gridX1 * ( iy + 1 );
            int c = numberOfVertices + ( ix + 1 ) + gridX1 * ( iy + 1 );
            int d = numberOfVertices + ( ix + 1 ) + gridX1 * iy;
            
            // faces
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(d);
            
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
            
            // increase counter
            groupCount += 6;
            
        }
        
    }
    
    // add a group to the geometry. this will ensure multi material support
    groups.push_back(IndexGroup(groupStart, groupCount, materialIndex));
    
    // calculate new start value for groups
    groupStart += groupCount;
    
    // update total number of vertices
    numberOfVertices += vertexCounter;
}

CubeGeometry::CubeGeometry(float width,float height,float depth,int widthSegments,int heightSegments,int depthSegments):width(width),height(height),depth(depth),widthSegments(widthSegments),heightSegments(heightSegments),depthSegments(depthSegments){
    
    // build each side of the box geometry
    buildPlane(2,1,0, - 1, - 1, depth, height, width, depthSegments, heightSegments, 0 ); // px
    buildPlane(2,1,0, 1, - 1, depth, height, - width, depthSegments, heightSegments, 1 ); // nx
    buildPlane(0,2,1, 1, 1, width, depth, height, widthSegments, depthSegments, 2 ); // py
    buildPlane(0,2,1, 1, - 1, width, depth, - height, widthSegments, depthSegments, 3 ); // ny
    buildPlane(0,1,2, 1, - 1, width, height, depth, widthSegments, heightSegments, 4 ); // pz
    buildPlane(0,1,2, - 1, - 1, width, height, - depth, widthSegments, heightSegments, 5 ); // nz


}

