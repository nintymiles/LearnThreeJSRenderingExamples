#ifndef TORUSKNOTMODEL_H
#define TORUSKNOTMODEL_H

#include <errno.h>
#include <vector>

#include "matrix4.h"
#include "ShadersAsst.h"

#include "Texture.h"
#include "Geometry.h"
#include "Camera.h"
#include "geometrymaker.h"


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

class TorusKnotModel{
    int32_t num_indices_;
    int32_t num_vertices_;
    GLuint vbo_;
    GLuint ibo_;
    
    GLuint vao_;
    
    std::shared_ptr<Geometry> geometry_;
    
    std::shared_ptr<PhongShaderState> phongShaderState_;
    
    std::shared_ptr<PerspectiveCamera> perspectiveCamera_;
    
    Matrix4 mat_projection_;
    Matrix4 mat_view_;
    
    Texture* texObj_  = nullptr;
    Texture* texObj1_  = nullptr;
    
    static float timer;
    
public:
    TorusKnotModel();
    virtual ~TorusKnotModel();
    void Init();
    void Render();
    void Update(double time);
    void Unload();
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    
    Matrix4 mat_model_;
    
};

#endif //TORUSKNOTMODEL_H
