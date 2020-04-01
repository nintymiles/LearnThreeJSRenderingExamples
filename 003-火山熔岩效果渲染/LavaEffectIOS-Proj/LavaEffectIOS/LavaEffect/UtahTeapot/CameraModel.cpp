//--------------------------------------------------------------------------------
// CameraModel.cpp
// Render a camera
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "CameraModel.h"

//--------------------------------------------------------------------------------
// Camera model data
//--------------------------------------------------------------------------------
#include "matrix4.h"
#include "Geometry.h"

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

//Constructor
CameraModel::CameraModel():vao_(0),vbo_(0),ibo_(0) {}

//Deconstructor
CameraModel::~CameraModel(){
    Unload();
}

void CameraModel::Init() {
    // Settings
    glFrontFace(GL_CCW);
    
    // Load shader
    simpleShaderState_.reset(new FrustumShaderState());
    
    std::vector<VertexPNX> vertices = loadObj("camera.obj");

    // Create VBO
    num_vertices_ = (int)vertices.size();
        
    geometry_.reset(new Geometry(&vertices[0],NULL,num_vertices_,0));
    
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPNX);
    // Pass the vertex data
    glVertexAttribPointer(CAMERA_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(CAMERA_ATTRIB_VERTEX);
    
    glVertexAttribPointer(CAMERA_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(CAMERA_ATTRIB_NORMAL);
    
    glBindVertexArray(0);
    
    mat_model_ = Matrix4::makeScale(Cvec3(0.15));
    
    mat_model_ =  mat_model_ * Matrix4::makeTranslation(Cvec3(16,-16,-6));
    mat_model_ =  mat_model_ * Matrix4::makeYRotation(180);
    mat_model_ =  mat_model_ * Matrix4::makeZRotation(45);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
    mat_view_ = inv(mat_view_);
    
    UpdateViewport();
}

void CameraModel::UpdateViewport() {
    int32_t viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    const float CAM_NEAR = -0.1f;
    const float CAM_FAR = -10000.f;
    
    
    float fov = 60;
    float aspect = static_cast<float>(viewport[2]) / static_cast<float>(viewport[3]);
    mat_projection_ =Matrix4::makeProjection(fov, aspect, CAM_NEAR, CAM_FAR);
}

void CameraModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
    if (ibo_) {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }
    
    //  if (shader_param_.program_) {
    //    glDeleteProgram(shader_param_.program_);
    //    shader_param_.program_ = 0;
    //  }
}

void CameraModel::Update(double time) {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void CameraModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update(0);
}

void CameraModel::Render(float r, float g, float b) {
    
    //mat_model_ = mat_model_ * Matrix4::makeZRotation(1);
    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    
    
    
    glUseProgram(simpleShaderState_->program);
    
    CAMERA_MATERIALS material = {
        {r, g, b}, {1.0f, 1.0f, 1.0f, 90.f}, {0.1f, 0.1f, 0.1f}, };
    
//    // Update uniforms
//    glUniform4f(simpleShaderState_->material_diffuse_, material.diffuse_color[0],
//                material.diffuse_color[1], material.diffuse_color[2], 1.f);
//
//    glUniform4f(simpleShaderState_->material_specular_, material.specular_color[0],
//                material.specular_color[1], material.specular_color[2],
//                material.specular_color[3]);
//
//    // using glUniform3fv here was troublesome
//
//    glUniform3f(simpleShaderState_->material_ambient_, material.ambient_color[0],
//                material.ambient_color[1], material.ambient_color[2]);
    
    GLfloat glmatrix[16];
    mat_vp.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(simpleShaderState_->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
//    GLfloat glmatrix2[16];
//    mat_view_.writeToColumnMajorMatrix(glmatrix2);
//    glUniformMatrix4fv(teapotShaderState_->matrix_view_, 1, GL_FALSE, glmatrix2);
//
//    glUniform3f(teapotShaderState_->light0_, -100.f, -200.f, 100.f);
    
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0,num_vertices_);
    
    glBindVertexArray(0);
}

