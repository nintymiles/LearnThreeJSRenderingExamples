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
    Cvec3 vertex;
    Cvec3 normal;
    
    Cvec3 P1,P2;
    
    Cvec3 B,T,N;
    
    // generate vertices, normals and uvs
    for (int i = 0; i <= tubularSegments; ++ i) {
        // the radian "u" is used to calculate the position on the torus curve of the current tubular segement
        
        float u = (float)i / tubularSegments * p * M_PI * 2;
        
        // now we calculate two points. P1 is our current position on the curve, P2 is a little farther ahead.
        // these points are used to create a special "coordinate space", which is necessary to calculate the correct vertex positions
        
        P1 = calculatePositionOnCurve(u, p, q, radius);
        P2 = calculatePositionOnCurve(u + 0.01, p, q, radius);
        
        // calculate orthonormal basis
        T = P2-P1;
        N = P2+P1;
        B = cross(T, N);
        N = cross(B, T);
        
        // normalize B, N. T can be ignored, we don't use it
        
        B.normalize();
        N.normalize();
        
        for (int j = 0; j <= radialSegments; ++ j ) {
            
            // now calculate the vertices. they are nothing more than an extrusion of the torus curve.
            // because we extrude a shape in the xy-plane, there is no need to calculate a z-value.
            
            float v = j / (float)radialSegments * M_PI * 2;
            float cx = - tube * cos(v);
            float cy = tube * sin(v);
            
            // now calculate the final vertex position.
            // first we orient the extrusion with our basis vectos, then we add it to the current position on the curve
            
            Cvec3 vertex;
            
            vertex[0] = P1[0] + (cx * N[0] + cy * B[0]);
            vertex[1] = P1[1] + (cx * N[1] + cy * B[1]);
            vertex[2] = P1[2] + (cx * N[2] + cy * B[2]);
            
            vertices.push_back(vertex);
            
            // normal (P1 is always the center/origin of the extrusion, thus we can use it to calculate the normal)
            
            Cvec3 normal=vertex - P1;
            normal.normalize();
            
            normals.push_back(normal);
            
            // uv
            Cvec2 uv(i / (float)tubularSegments,j / (float)radialSegments);
            uvs.push_back(uv);
            
        }
        
    }
    
    // generate indices
    for (int j = 1; j <= tubularSegments; j ++ ) {
        
        for (int i = 1; i <= radialSegments; i ++ ) {
            
            // indices
            
            int a = ( radialSegments + 1 ) * ( j - 1 ) + ( i - 1 );
            int b = ( radialSegments + 1 ) * j + ( i - 1 );
            int c = ( radialSegments + 1 ) * j + i;
            int d = ( radialSegments + 1 ) * ( j - 1 ) + i;
            
            // faces
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
    
    // build geometry
    
//    this.setIndex( indices );
//    this.addAttribute( 'position', new Float32BufferAttribute( vertices, 3 ) );
//    this.addAttribute( 'normal', new Float32BufferAttribute( normals, 3 ) );
//    this.addAttribute( 'uv', new Float32BufferAttribute( uvs, 2 ) );
    

}

