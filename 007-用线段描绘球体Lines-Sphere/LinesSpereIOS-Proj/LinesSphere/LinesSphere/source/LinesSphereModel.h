#ifndef LINESSPHEREMODEL_H
#define LINESSPHEREMODEL_H

#include <errno.h>
#include <vector>

#include "matrix4.h"
#include "ShadersAsst.h"

#include "Texture.h"
#include "Geometry.h"
#include "Camera.h"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))


struct Model{
    Cvec3 position;
    Cvec3 rotation;
    Cvec3 scale;
    
};

class LinesSphereModel:public Model{
    int32_t num_vertices_;
    GLuint vbo_;
    
    GLuint vao_;
    
    std::shared_ptr<Geometry> geometry_;
    
    std::shared_ptr<SimpleShaderState> simpleShaderState_;
    
    std::shared_ptr<PerspectiveCamera> perspectiveCamera_;
    
    Matrix4 mat_projection_;
    
    
    Texture* texObj_  = nullptr;
    Texture* texObj1_  = nullptr;
    
    static float timer;
    
    
    
public:
    LinesSphereModel(){};
    LinesSphereModel(std::shared_ptr<Geometry> geometry,std::shared_ptr<SimpleShaderState> simpleShaderState);
    virtual ~LinesSphereModel();
    void Init();
    void Render();
    void Update();
    void UpdateMatrixWorld();
    void Unload();
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    
    Cvec3 scale;
    Cvec3 rotation;
    Cvec3 position;
    Matrix4 mat_model_;
    Matrix4 mat_view_;
    Cvec3 lineColor;
    GLfloat lineOpacity;
    bool isPickingEnabled=true;
    
};

#endif //LINESSPHEREMODEL_H
