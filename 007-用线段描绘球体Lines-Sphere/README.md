# 用线段绘制球体（three.js webgl_lines_spere例子）
Three.js中的webgl_lines_sphere例子使用线段渲染出3D球体，效果十分有趣。

## 实现过程
渲染过程比较简单，先构造出单位球体上（unit sphere）的点，随后将点再对应半径上随机伸缩坐标，每个半径上相邻的这两个点构成线段，为线段指定色彩后即可渲染出基本的lines sphere（线段球体）。

这里，我们模仿webgl_lines_sphere例子，使用C++和OpenGL ES 3.0获得了如下的渲染效果，[iOS版本实现源码](https://github.com/nintymiles/LearnThreeJSRenderingExamples)可以从github上获取。

![lines_sphere_ani](media/lines_sphere_anim.gif)


## 用线段绘制一个基本的3D球体
这一过程的关键在于使用线段代替顶点形成球形几何体数据。首先，我们使用随机数分别产生范围为[-1..1]的$[x,y,z]^t$各个坐标部件的数值,然后把这些坐标看作矢量，并进行标准化。随后，我们再将这些矢量的坐标看作点的坐标，那么这些点将位于中心为原点的单位球体之上。如此，我们就拥有了一个球体的顶点数据，之后，再针对这个顶点沿着半径方向(也是这个点的法线法线方向）向外在一定范围内随机伸缩这个点形成单位球体外围的点，将这个点和单位球体上同半径的点连接为线段。这样，我们就拥有了近似绘制球体的线段顶点数据。在渲染时，针对线段指定代表色彩和透明度的uniform变量即可渲染出一个线段球体。生成顶点数据的代码如下：

```cpp
vector<Cvec3f> createLinesSphereGeometry(float r){
    vector<Cvec3f> vertices;
    
    for(int i = 0; i < 1500; i++) {
        //生成范围为[-1..1]的随机坐标部件值
        float x = rand()/(RAND_MAX/2.0) - 1;
        float y = rand()/(RAND_MAX/2.0) - 1;
        float z = rand()/(RAND_MAX/2.0) - 1;
        
        Cvec3f vertex(x,y,z);
        
        //标准化后，这些随机点就变成了单位球体上的点，经过缩放后，可以变成了不同尺寸球面上的点
        vertex.normalize();
        //缩放为半径为r的球体
        vertex *= r;
        
        vertices.push_back(vertex);
        
        //在当前半径为r的球体上的点，被乘以固定方位的随机因子
        Cvec3f vertexOuter = vertex * ((rand()/(float)RAND_MAX) * 0.09 + 1 );
        
        vertices.push_back(vertexOuter);
        
    }
    
    return vertices;
}
```

## 球体的多重绘制和运动
webgl_lines_sphere例子中进行了多重（9层）球体绘制，并且让内层的球体随时间缩放变换，同时进行逆时针旋转，外侧的球体则只进行顺时针旋转。还有，内外侧球体的色彩和透明度也进行对应的设置。整体的展示效果比较有趣。部分代码如下：

```cpp

struct RenderingParameter{
    float scale;
    string color;
    float opacity;
};
//球体的初始大小，色彩和透明度信息
vector<RenderingParameter> parameters = {{0.25,"0xff7700", 1}, {0.5, "0xff9900", 1}, {0.75, "0xffaa00", 0.75}, {1, "0xffaa00", 0.5}, {1.25, "0x000833", 0.8},
    {3.0, "0xaaaaaa", 0.75}, {3.5,"0xffffff",0.5}, {4.5, "0xffffff", 0.25}, {5.5, "0xffffff", 0.125}};

//球体的初始化过程代码
shared_ptr<Geometry> geometry;
vector<Cvec3f> lineSphereVertices = createLinesSphereGeometry(300.0);
geometry.reset(new Geometry(&lineSphereVertices[0],(int)lineSphereVertices.size()));
   
mainCamera.reset(new PerspectiveCamera());
mainCamera->updatePorjectonMatrix();
    
Cvec3 translationVec = Cvec3(0,0,-550);
    
for(size_t i=0;i<parameters.size();i++){
	shared_ptr<LinesSphereModel> ls = make_shared<LinesSphereModel>(LinesSphereModel(geometry,simpleShaderState));
	    
	RenderingParameter para=parameters[i];
	//每个球体的初始大小
	ls->scale = Cvec3(para.scale,para.scale,para.scale);
	originalScale[i]=para.scale;
	ls->position = translationVec;
	    
	//每个球体的色彩    
	Cvec3 lColor = hexStringToRGB(para.color);
	ls->lineColor = lColor;
	    
	//每个球体的初始选择角度（围绕y轴）    
	float angleY = rand()/(RAND_MAX/180.0);
	ls->rotation = Cvec3(0,angleY,0);
	    
	lsModels.push_back(ls);
}

//每次绘制时球体的运动设置

//全局变量，在每帧绘制时增加固定数量
delta+=0.1;

for(int i=0;i<lsModels.size();i++){

    shared_ptr<LinesSphereModel> lsModel = lsModels[i];

    //设置内外圈球体的旋转角度
    int factor = i<4?(i+1):-(i+1);
    long rotationY = delta*(factor);
    lsModel->rotation = Cvec3(0,rotationY,0);

	  //对内圈球体在特定范围内以正弦方式循环缩放
    if (i < 5){
        float scale = originalScale[i] * (i/5.0 + 1) * (1 + 0.5 * sin(7 * delta * 0.05));
        lsModel->scale = Cvec3(scale,scale,scale);
    }

    lsModel->setPerspectiveCamera(mainCamera);
    lsModel->UpdateMatrixWorld();
    lsModel->Render();
}
```





