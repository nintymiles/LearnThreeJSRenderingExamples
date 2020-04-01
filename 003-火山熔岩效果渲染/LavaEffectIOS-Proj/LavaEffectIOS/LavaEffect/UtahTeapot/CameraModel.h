#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#include <errno.h>
#include <vector>

#include "matrix4.h"
#include "ShadersAsst.h"

#include "Geometry.h"
#include "Camera.h"


#define BUFFER_OFFSET(i) ((char*)NULL + (i))

enum CAMERA_ATTRIBUTES {
    CAMERA_ATTRIB_VERTEX,
    CAMERA_ATTRIB_NORMAL,
};

struct CAMERA_MATERIALS {
    float diffuse_color[3];
    float specular_color[4];
    float ambient_color[3];
};

class CameraModel{
    int32_t num_indices_;
    int32_t num_vertices_;
    GLuint ibo_;
    GLuint vbo_;
    
    GLuint vao_;
    
    std::shared_ptr<Geometry> geometry_;
    
    std::shared_ptr<FrustumShaderState> simpleShaderState_;
    
    std::shared_ptr<PerspectiveCamera> perspectiveCamera_;
    
    Matrix4 mat_projection_;
    Matrix4 mat_view_;
    
    
public:
    CameraModel();
    virtual ~CameraModel();
    void Init();
    void Render(float r, float g, float b);
    void Update(double time);
    void Unload();
    void UpdateViewport();
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    
    Matrix4 mat_model_;
    
};

#endif
