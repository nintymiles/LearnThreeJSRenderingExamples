# 渲染物体用户交互raycaster（three.js webgl_interactive_cube例子）
three.js中的picking（选中渲染物体）方式使用了ray caster方式实现了用户交互。用户可以在一大群随机立方体上准确选择出一个具体的立方体。

## 实现过程
首先渲染出立方体群。一开始使用随机数生成特定范围的位置，伸缩尺寸，色彩等信息，随后使用这些信息渲染出立方体群。立方体渲染完成后，我们还会加上眼睛围绕立方体群的圆周运动，产生不断变化的视图。

立方体群渲染完成后，我们使用raycaster在鼠标点击处投射出屏幕射线，如果屏幕射线和被渲染物体相交，则认为这个物体被选中。


## 第一步







## 实现raycast picking


### Pick的基本原理？
three.js的基本原理是使用mesh等对象包含物体对象，然后在mesh中集成raycaster机制

### Picking机制的理解
首先要先学习理论，在韩国版本的OpenGL ES书中有一章picking章节，讲解picking的原理，要仔细理解。

#### Screen-Ray
屏幕射线的定义：用户点击屏幕像素（x_s,y_s)，发出一条screen-ray，方向为（0，0，1）。

**重要点：所有的屏幕空间射线都是平行线，方向都是朝向正z轴(0,0,1)，和物体进入眼睛的光线正好相反。** 当然这是一种默认设置，OpenGL API中，默认的NDC空间是left handed coordinate system。屏幕空间射线射向物体。

这个定义跟我所理解的不一样，我最开始认为屏幕射线会从原点出发，方向为从原点到这个屏幕点。

屏幕射线这么定义的原因在于，从顶点到原点的射线从clip space转化为ndc space后，变为平行线。所以这个**反向的屏幕射线这么定义是完全准确的**。

##### Screen点和screen-ray的变换
screen点(x_s,y_s,0)和screen-ray(0,0,1)通过反转的viewport矩阵和投射矩阵，被变换为(x_c,y_c,-n)，注意这种投射刚好将屏幕点投射到近平面之上，所以这个z=n是确定的。通过这个变换可以确定出clip space中近平面上的点(x_c,y_c,-n)的计算公式。当这个点确定后，用这个点减去原点，就得到screen-ray在clip space的方向矢量，然后由于z轴比为n，可以消除n的存在。

随后应用eye matrix，将它们转化为world coordinate。然后在根据需要转后为不同的object coordinate。

##### ThreeJs中的屏幕射线计算
Three.js中的screen-ray的计算步骤：

1. 首先确定eye frame的原点（相机在world frame中的位置）
2. 随后计算鼠标像素点坐标在world frame中的坐标
3. 最后，在world frame中使用鼠标点击点的世界坐标减去相机位置，标准化后得到方向矢量。

##### Three.js中的一些基础设施

1. Mesh中包含物体的集合数据，geometry，从geometry可以确定BV。
2. Mesh中提供有object matrix（worldMatrix，不同的叫法），提供从object coordinate到world coordinate的转换 。
3. 使用object matrix将几何体的BV转换为world coordinate。
4. Mesh负责生成raycaster。
5. raycaster采集鼠标数据，利用对应矩阵生成screen-ray（raycaster.setFromCamera())。
6. ray的封装中包含ray-sphere，和ray-box的基础计算

##### Three.js中geometry对象所包含的用于raycast的基础设施支持
geometry对象包含了boundingSphere属性，作为BV(bounding volume)的实现。boundingSphere为一个Sphere对象，其有一个中心点和一条半径来决定。

**tighter sphere的确定：** boundingSphere的中心点通过使用顶点先构成AABB箱体，然后AABB的中心点为其x，y，z轴坐标的中件差值。随后在通过顶点遍历计算到中心点的最大距离，使用其作为sphere的半径，从而形成一个tighter sphere，而不是使用基于AABB顶点sphere的poor-fit sphere。

##### Ray对象中ray-sphere和ray-box的基础计算
real-time rendering中对于具体的实现算法讲解的更加清楚。

###### BoundingVolume的确定逻辑
**使用顶点表达的BoundingVolume，不管是sphere还是box都比较容易直接确定。但是使用mesh表达的几何数据，其bounding sphere和box还需要根据mesh的数据特定经过特定计算才能确定出sphre和box。这里还有比较复杂的逻辑**

##### Three.js中探测ray-geometry相交的算法原理
首先，每个mesh对象包含了物体的几何数据（不管是buffer，还是简单几何数据），mesh还包含了raycaster对象生成机制，用于监管像素射线screen-ray的生成。最终，每个mesh使用raycaster对象执行raycast动作探测screen-ray和几何对象是否相交。

screen-ray相交的执行逻辑：

1. 首先生成几何对象的BV-bounding sphere，执行ray-sphere相交保守检测，如果不相交则舍弃整个检查。
2. 之后还会执行ray-box相交检测，再一次保守检测
3. 最后执行ray-triangle相交，这设计到ray和每个三角形的相交检测，在cpu上执行。ray-triangle相交检测看似简单，其实技术难度最大。要先计算barycentric坐标。其中参数,u+v+q=1，要都大于0，ray才会和三角形相交，一般ray和几何体相交，相交点都会多余1个，最小的t值为最先相交点。

##### 对three.js中的raycast picking机制应用于solar system simulation的考虑。
在Model对象中执行raycast检测机制，给予Model实例一个raycaster对象，用于初始化screen-ray射线，同时执行raycast动作。Model对象中应该有一个共用方法，可以执行各个子对象的raycast动作。

RayCaster拥有ray，执行raycast动作，raycaster是对所有的几何对象执行raycast操作。screen-ray对于不同的物体几何对象是相同的。那么raycaster可以设计为Model的静态实例，用于对所有的model执行raycast动作。
raycaster中可以包含ray对象，具体的ray对象用于存储屏幕射线，并且包含ray-bv的相关实现方法。

**RayCaster一般在场景中初始化，利用鼠标数据和场景的camera设置Ray。然后在渲染时执行intersectObject动作，通常是在点击事件中执行raycast动作，并且处理raycast结果，比如让被选中的物体的着色发生改变（半透明或某种纯色之类）**

#####  three.js对对ray-bv相交返回的intersection结构
three.js中返回的intersection结构整理：
intersection={
float distance,
vec3 intersectionPointWorld,
Model* object,
int faceIndex, // not yet
vec3 face, //not yet
vec3 uv, // not yet
}

整体相交检查的返回结果为：
vector<intersection> intersects;

##### 每个被相交的model对象如何支持☑️选中渲染？
需要配套相关的机制，这个机制包括，每个被渲染对象在每次渲染前不应该是重新生成的，否则设置选中模式会有些麻烦。

另外对特定层级的model设置关闭选中模式，这可以通过isPickingEnabled属性方法来决定。当启用时，不参与picking动作，也不参与intersected渲染。





