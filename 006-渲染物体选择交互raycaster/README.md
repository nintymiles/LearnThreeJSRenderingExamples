# 渲染物体raycast picking拾取交互（three.js webgl_interactive_cube例子）
Three.js中的webgl_interactive_cube例子展示了picking-拾取物体（使用点击等方式选中渲染物体）的行为。在这里借助ray casting方式实现了用户交互。用户可以在一大群随机立方体上准确选择出一个具体的立方体。

## 实现过程
首先渲染出立方体群。一开始使用随机数生成特定范围的位置，伸缩尺寸，色彩等信息，随后使用这些信息渲染出立方体群。我们还会加上眼睛围绕立方体群做圆周运动，产生不断变化的视图。

立方体群渲染完成后，我们使用raycaster在鼠标点击处投射出屏幕射线，如果屏幕射线和被渲染物体相交，则认为这个物体被选中。

这个例子比较复杂，包含多个子过程：立方体几何模型数据生成，单个立方体渲染，立方体群渲染，眼睛帧围绕场景中心做圆周运动，用户拾取机制等。

这里，我们模仿webgl_interactive_cube例子，在application端计算射线与物体相交的方式来决定被选中的立方体，使用C++和OpenGL ES 3.0获得了如下的渲染效果，[iOS版本实现源码](https://github.com/nintymiles/LearnThreeJSRenderingExamples)可以从github上获取。
![interactive_cubes_picked](media/interactive_cubes_picked.gif)



## 实现立方体群的渲染
### 基本立方体的绘制
要实现立方体的绘制，首先需要准备立方体模型的顶点数据。在webgl_interactive_cube例子中并没有使用硬编码的顶点数据，而是实现了CubeGeometry对象，可以用于灵活控制生成几何体顶点数据的数量。CubeGeometry对象生成的几何顶点数据包含顶点和索引。下面为C++实现的代码局部：

```cpp
//构建立方体每个面的数据
    buildPlane(2,1,0, - 1, - 1, depth, height, width, depthSegments, heightSegments, 0 ); // px
    buildPlane(2,1,0, 1, - 1, depth, height, - width, depthSegments, heightSegments, 1 ); // nx
    buildPlane(0,2,1, 1, 1, width, depth, height, widthSegments, depthSegments, 2 ); // py
    buildPlane(0,2,1, 1, - 1, width, depth, - height, widthSegments, depthSegments, 3 ); // ny
    buildPlane(0,1,2, 1, - 1, width, height, depth, widthSegments, heightSegments, 4 ); // pz
    buildPlane(0,1,2, - 1, - 1, width, height, - depth, widthSegments, heightSegments, 5 ); // nz
    
 //本函数生成立方体每个面上的顶点后索引数据
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
    
    ... ...
}   
```
立方几何体数据生成后，使用phong shader借助GL_TRIANGLES进行基本立方体的渲染。

### 渲染立方体群
要实现立方体群的渲染，重点是每个立方体的位置，方位，以及色彩等属性的确定，以便于生成的每个基本立方体都可以辨别区分。

这些属性信息的生成代码如下：

```cpp
... 
int count = 1000;
    
for (int i = 0; i < count; i ++ ) {
	double x = rand()%50 - 25;
	double y = rand()%50 - 25;
	double z = rand()%50 - 25;
	mInstancePos.push_back(Cvec3(x,y,z));
	mInstanceOrientation.push_back(Cvec3(rand()%360,rand()%360,rand()%360));
	mInstanceScale.push_back(Cvec3((rand()%100)/100.0+0.5,(rand()%100)/100.0+0.5,
	                           (rand()%100)/100.0+0.5));
    
	//random diffuse color
	int cInt = 0xffffff * (rand()%100/(100.0));
	std::stringstream stream;
	stream << std::hex << cInt;
	std::string hexString( stream.str() );
	Cvec3 diffuseColor = hexStringToRGB(hexString);
	mInstanceDiffuseColor.push_back(diffuseColor);
	
}
...

```

当然还有一个主要问题，就是大量的几何体如何实现有效的渲染,比如怎样保证几千个立方体在较高的帧率（FPS）下渲染。基本动作包括避免无效的API调用开销，避免图形缓存（vbo,ibo,vao)的无效损耗，避免纹理的过多加载。在渲染立方体群时，每个立方体都可以使用相同的vbo/ibo缓存，使用相同的渲染管线（program），借助vao管理vertex array设置等等。同时，尽量在application端避免不必要的矩阵计算，比如每个立方体的位置和方位信息，不必要每次渲染时都执行计算。

### 眼睛（相机）视野的变化
当立方几何体群按照随机位置、方位、缩放程度等属性生成后，保持不变。我们通过移动眼睛（相机）的方式，保持画面的变换。当移动眼睛（眼睛帧）时，我们保持眼睛持续看向场景的原点，同时围绕球体不断移动眼睛的位置。所有的立方体在渲染时使用相同的view matrix。示意代码如下：

```cpp
	  
	  angle+=0.1;
    float radius = 10;
    float eyeX = radius * sin(angle*M_PI/180);
    float eyeY = radius * sin(angle*M_PI/180);
    float eyeZ = radius * cos(angle*M_PI/180);
    Matrix4 mat_eye = Matrix4::makeLookAtEyeMatrix(Cvec3(eyeX,eyeY,eyeZ), Cvec3(0,0,0), Cvec3(0,1,0));
    
    Matrix4 viewMat = inv(mat_eye);
    
    cubeModel->mat_view=viewMat;
```

## 实现raycast picking(射线投射拾取）
webgl_interactive_cube例子中采用射线拾取(raycast picking)方式。这种方式通过发出一条屏幕射线（screen ray），然后检测屏幕射线是否和被渲染物体（boudning volumes）相交。如果相交，则物体被选中，反之，则未选中。

### 屏幕射线（Screen Ray）
屏幕射线的定义：用户点击屏幕像素（$x_s,y_s$)，发出一条screen ray，方向为（0，0，1）。
**所有的屏幕空间射线都是平行线，方向都是朝向正z轴(0,0,1)，和物体进入眼睛的光线正好相反。** 当然这是一种默认设置，OpenGL API中，默认的NDC空间是left handed coordinate system。屏幕射线总是射向物体。

这个定义跟通常所理解的稍有不同，我最开始认为屏幕射线会从原点出发，方向为从原点到这个屏幕点。实际上，屏幕射线只是起点反向延伸到原点。

屏幕射线这么定义的原因在于，从顶点到原点的射线从clip space转化为ndc space后，变为平行线。所以这个**反向的屏幕射线这么定义是完全准确的**。

#### Screen像素点和screen ray的坐标变换
screen点($x_s,y_s,0$)和screen-ray(0,0,1)通过反转的viewport矩阵和投射矩阵，被变换为($x_c,y_c,-n$)，注意这种投射刚好将屏幕点投射到近平面之上，所以这个z=n是固定的。通过这个变换可以确定出clip space中近平面上的点($x_c,y_c,-n$)的计算公式。当这个点确定后，用这个点减去原点，就得到screen-ray在clip space的方向矢量，然后由于z轴比为n，可以消除n的存在。

随后应用eye matrix（view matrix的反转），将它们转化为world coordinate。然后再根据需要将其转换为对应的object coordinate。

#### 屏幕射线的计算步骤
1. 首先确定相机（eye frame）的原点（相机在world frame中的位置）
2. 随后计算点击像素点坐标在world frame中的坐标
3. 最后，在world frame中使用点击的像素点的世界坐标减去相机位置，标准化后得到方向矢量。

#### 屏幕射线在应用中的设置
我们通常会生成一个RayCaster对象，这个对象用于生成屏幕射线。在应用中，我们通常在点击或者鼠标事件中初始化RayCaster对象，RayCaster对象利用被点击屏幕像素的坐标初始化出一条屏幕射线。一般情况下，屏幕射线会以世界坐标来表示，随后所进行的射线和几何体相交检测就会利用这条射线进行。生成屏幕射线的部分代码如下：


```cpp
    
    //借助相机数据生成world frame中的screen-ray
    //屏幕点先是被对应为near plane上的点，然后再转换为world coordinate
    //也就是说先进行反转viewport计算，然后在应用unporjection矩阵，最后应用eye matrix（inverse view matrix）
    void setFromCamera(Cvec3 screenPos,shared_ptr<PerspectiveCamera> camera){
    		//反转viewport计算，由于窗口坐标y轴的原点在窗口上方，所以需要反转符号
        float rayOriginX = (screenPos[0]/camera->view.width) * 2 - 1;
        float rayOriginY = -(screenPos[1]/camera->view.height) * 2 + 1;
        
        if(camera){
            Matrix4 eyeMat = camera->eyeMat;
            Matrix4 projMat = camera->projMat;
            Cvec3 camPosition = vec3(eyeMat(0,3),eyeMat(1,3),eyeMat(2,3));
            Cvec4 screenPosWorld= (eyeMat*inv(projMat)) * vec4(rayOriginX,rayOriginY,1.0f,1.0f);
            //反转投射矩阵应用后的坐标仍为同质坐标，需要执行除法以获得放射坐标
            screenPosWorld = screenPosWorld/screenPosWorld[3];
            //screen ray方向矢量需要标准化
            Cvec3 rayDi =normalize(vec3(screenPosWorld) -camPosition);
            
            ray = new Ray(camPosition,rayDi);
        }
        
    }
```

### 射线和几何体相交检测(ray-geometry intersection test)
射线和几何体相交检测实际是要计算出屏幕射线和构成几何体的所有三角形是否相交，只要射线和其中至少一个三角形相交，我们就认为射线和几何体相交。但是由于构成场景的几何体以及几何体本身的三角形数目很多，所以直接进行射线三角形（ray-triangle）相交检测的开销很大。通常的方式是使用绑定容积（bounding volume）进行保守检测计算。bounding volume通常为围绕几何体的球体（sphere）或者方形体（box），这些sphere或者box一般根据几何体的顶点数据近似获得。

使用几何体的顶点表达来获得Bounding Volume，不管是sphere还是box，比较容易确定。box一般是最大/最小顶点所构成的AABB box。sphere一般先使用AABB box确定中心点的坐标，然后计算这个中心点和各个顶点的最大距离作为球体半径，这样所获得的球体一般为更紧凑的球体（tighter sphere）。

#### ray-geometry相交检测的原理
屏幕射线和几何体相交检测通常先执行ray-sphere或者ray-box保守检测，如果检测不通过，则后续不会再执行开销大的ray-triangle检测。基本的执行逻辑如下：

1. 首先生成一个几何体的bounding sphere，执行ray-sphere相交保守检测，如果不相交则舍弃整个检查。
2. 之后再执行几何体的ray-box相交检测，再一次保守检测，逐渐精确。
3. 最后执行ray-triangle相交，这涉及到屏幕射线和每个三角形的相交检测，在cpu上执行。ray-triangle要先计算barycentric坐标u,v,w,其中u+v+q=1，u,v,w要都大于0，射线才会和三角形相交，一般射线和几何体相交，相交点可能会大于1个，最小的t值为最先相交点。此处所计算出的相交点的精度是比较高的。

ray-bv和ray-triangle检测的部分移值代码：

```cpp
vector<vec3> Ray::intersectSphere(Sphere* sphere){
    
    vec3 v1;
    //两个矢量相减产生新矢量v1，球体原点和射线原点的矢量
    v1 = sphere->center - origin;
    
    //使用标准矢量和非标准矢量的点积来计算余弦边。
    float tca = dot(v1,direction);

    //d2是正弦边的平方，v1平方构成从相机位置和球体中心为最长边平方，tca2为余弦边的平方，
    float d2 = dot(v1,v1) - tca * tca;
    //当d2和radius2刚好相等时，屏幕射线为球体切线，d2>raidus2时，屏幕射线和球体不相交
    float radius2 = sphere->radius * sphere->radius;
    
    //screen-ray和球体不相交
    if ( d2 > radius2 ) return {};
    
    float thc = sqrt( radius2 - d2 );
    
    // t0*ray方向就等于从相机原点到球体表面相交点的距离
    // t0 = first intersect point - entrance on front of sphere
    float t0 = tca - thc;
    // t1为到远距离点的距离
    // t1 = second intersect point - exit point on back of sphere
    float t1 = tca + thc;
    
    // test to see if both t0 and t1 are behind the ray - if so, return null
    if ( t0 < 0 && t1 < 0 ) return {};
    
    // test to see if t0 is behind the ray:
    // if it is, the ray is inside the sphere, so return the second exit point scaled by t1,
    // in order to always return an intersect point that is in front of the ray.
    if ( t0 < 0 ) return {this->at(t1)};
    
    // else t0 is in front of the ray, so return the first collision point scaled by t0
    return {this->at(t0)};
}

vector<vec3> Ray::intersectBox(Box* box){
    
    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    
    float invdirx = 1 / direction[0],
        invdiry = 1 / direction[1],
        invdirz = 1 / direction[2];
    
    if(invdirx >= 0){
        tmin = (box->min[0] - origin[0]) * invdirx;
        tmax = (box->max[0] - origin[0]) * invdirx;
    }else{
        tmin = (box->max[0] - origin[0]) * invdirx;
        tmax = (box->min[0] - origin[0]) * invdirx;
    }
    
    if(invdiry >= 0 ) {
        tymin = (box->min[1] - origin[1]) * invdiry;
        tymax = (box->max[1] - origin[1]) * invdiry;
    } else {
        tymin = (box->max[1] - origin[1]) * invdiry;
        tymax = (box->min[1] - origin[1]) * invdiry;
    }
    
    if ( ( tmin > tymax ) || ( tymin > tmax ) ) return {};
    
    // These lines also handle the case where tmin or tmax is NaN
    // (result of 0 * Infinity). x !== x returns true if x is NaN
    if ( tymin > tmin || tmin != tmin ) tmin = tymin;
    
        if ( tymax < tmax || tmax != tmax ) tmax = tymax;
            
            if ( invdirz >= 0 ) {
                tzmin = ( box->min[2] - origin[2] ) * invdirz;
                tzmax = ( box->max[2] - origin[2] ) * invdirz;
            } else {
                tzmin = ( box->max[2] - origin[2] ) * invdirz;
                tzmax = ( box->min[2] - origin[2] ) * invdirz; 
            }
    
    if ( ( tmin > tzmax ) || ( tzmin > tmax ) ) return {};
    
    if ( tzmin > tmin || tmin != tmin ) tmin = tzmin;
        
        if ( tzmax < tmax || tmax != tmax ) tmax = tzmax;
            
            //return point closest to the ray (positive side)
            if ( tmax < 0 ) return {};
    
    return {this->at( tmin >= 0 ? tmin : tmax)};
}

vector<vec3> Ray::intersectTriangle(vec3 a,vec3 b,vec3 c,bool backfaceCulling){
    
    // Compute the offset origin, edges, and normal.
    vec3 diff,edge1,edge2,normal;
    
    // from http://www.geometrictools.com/GTEngine/Include/Mathematics/GteIntrRay3Triangle3.h
    edge1 = b-a;
    edge2 = c-a;
    normal = cross(edge1, edge2);
        
    //cross prodcut可以十分方便地应用于determiant的计算
    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N) 
    //确定符号，是因为要计算位于正反面
    float DdN = dot(direction,normal);
    int sign;
        
    if (DdN > 0){
        if (backfaceCulling) return {};
        sign = 1;
    }else if( DdN < 0 ){
        sign = - 1;
        DdN = - DdN;
    } else {
        return {};
    }
        
    //此处Q指向相机原点，于korea text方向相反，故而后面需要负值。
    diff = origin-a;
    
    float DdQxE2 = sign * dot(direction,cross(diff, edge2));
        
    // b1 < 0, no intersection
    if ( DdQxE2 < 0 ) {
        return {};
    }
        
    float DdE1xQ = sign * dot(direction,cross(edge1,diff));
        
    // b2 < 0, no intersection
    if ( DdE1xQ < 0 ) {
        return {};
    }
        
    // b1+b2 > 1, no intersection
    if ( DdQxE2 + DdE1xQ > DdN ) {
        return {};
    }
        
    // Line intersects triangle, check if ray does.
    float QdN = - sign * dot(diff,normal);
        
    // t<0,则位于射线的反方向上？
    // t < 0, no intersection
    if ( QdN < 0 ) {
        return {};
    }
        
    // t值确定后，就可以确定相交点。
    // Ray intersects triangle.
    return {this->at(QdN/DdN)};

}
```

> real-time rendering书中对于具体的射线几何体相交实现算法有十分详细的讲解。

每条屏幕射线可能和场景中的多个物体相交。在本例中，我们只选择最近相交的物体给予展示。





