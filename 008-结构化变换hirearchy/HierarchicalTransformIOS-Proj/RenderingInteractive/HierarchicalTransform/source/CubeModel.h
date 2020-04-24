#ifndef CUBEMODEL_H
#define CUBEMODEL_H

#include <errno.h>
#include <vector>

#include "matrix4.h"
#include "ShadersAsst.h"

#include "Texture.h"
#include "Geometry.h"
#include "Camera.h"

#include "Object.h"
#include "geometrymaker.h"
#include "Sphere.h"
#include "Ray.h"
#include "MatrixStack.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))


enum TEXTURE_SHADER_ATTRIBUTES {
    TEXTURE_ATTRIB_VERTEX,
    TEXTURE_ATTRIB_NORMAL,
    TEXTURE_ATTRIB_UV,
};

struct TORUS_MATERIALS {
    float diffuse_color[3];
    float specular_color[4];
    float ambient_color[3];
};

struct Model:public Object{
    Cvec3 position;
    Cvec3 rotation;
    Cvec3 scale;
    
    Sphere* boundingSphere;
    
    Model(Object *parent):Object("",parent){
    }
};

class CubeModel;
//基础相交数据结构定义，如果针对mesh结构几何数据，可能还不足够
struct IntersectionData {
    float distance;
    Cvec3 intersectionPointWorld;
//    shared_ptr<CubeModel> object;
    CubeModel* object;
};

class RayCaster;
class CubeModel:public Model{
    int32_t num_indices_;
    int32_t num_vertices_;
    GLuint vbo_;
    GLuint ibo_;
    
    GLuint vao_;
    
    std::shared_ptr<Geometry> geometry_;
    
    //std::shared_ptr<PhongShaderState> shaderState_;
    std::shared_ptr<NormalColorShaderState> shaderState_;
    
    std::shared_ptr<PerspectiveCamera> perspectiveCamera_;
    
    Matrix4 mat_projection_;
    
    
    Texture* texObj_  = nullptr;
    Texture* texObj1_  = nullptr;
    
    static float timer;
    std::vector<Cvec3> mInstancePos;
    std::vector<Cvec3> mInstanceOrientation;
    
    
    
public:
    //CubeModel(){};
    CubeModel(Object *parent,std::shared_ptr<Geometry> geometry,std::shared_ptr<NormalColorShaderState> phoneShaderState);
    virtual ~CubeModel();
    void Init();
    void Render();
    void Update();
    void UpdateMatrixWorld();
    void Unload();
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    void ApplyModelsTransformation();
    
    vector<IntersectionData> rayCast(RayCaster *rayCaster);
    
    Matrix4 mat_model_;
    Matrix4 mat_view_;
    shared_ptr<MatrixStack> matrixStatck;
    Cvec3 diffuseColor;
    bool isPickingEnabled=true;
    
};

#endif //CUBEMODEL_H
