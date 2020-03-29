# 视图平截锥体（view frustum）几何体渲染
threejs中webgl_camera例子中渲染出了视图平截锥体的形状，其实现了frustum geometry对象用于专门负责view frustum几何体数据的生成。

下面我们仿照threejs中camera的实现原理使用OpenGL ES在iOS上实现view frustum几何形状的渲染。代码可从github中获取。

## 视图平截锥体（view frustum）几何体数据的生成
在OpenGL中，投射矩阵将视图平截锥体变换为经典立方体（经透视除法-perspective division-之后）。如果要渲染view frustum，关键在于如何生成view frustum几何体。如果我们在眼睛空间直接描述，则面临着针对每一种不同的view frustum（投射矩阵参数所确定），都要根据参数重新指定view frustum的各个顶点。


