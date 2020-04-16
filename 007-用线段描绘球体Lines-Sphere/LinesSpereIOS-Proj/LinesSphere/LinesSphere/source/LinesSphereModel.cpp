//--------------------------------------------------------------------------------
// LinesSphereModel.cpp
// Render a lines sphere
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "LinesSphereModel.h"

#include "matrix4.h"
#include "Geometry.h"


#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <cmath>
#include <sstream>

#define ATTRIB_VERTEX 0

float LinesSphereModel::timer=0.f;

LinesSphereModel::LinesSphereModel(std::shared_ptr<Geometry> geometry,std::shared_ptr<SimpleShaderState> simpleShaderState):geometry_(geometry),simpleShaderState_(simpleShaderState) {
    vao_ = 0;
    vbo_ = 0;
    
   
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(Cvec3f);
    // Pass the vertex data
    glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(ATTRIB_VERTEX);
    

    
    glBindVertexArray(0);
    
}

LinesSphereModel::~LinesSphereModel(){
    Unload();
}

void LinesSphereModel::Init() {
    Matrix4 mat_eye = Matrix4::makeTranslation(Cvec3(0,0,4));
    mat_view_ = inv(mat_eye);
}

void LinesSphereModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
}

void LinesSphereModel::Update() {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void LinesSphereModel::UpdateMatrixWorld() {
    Matrix4 mat_object = Matrix4::makeXRotation(rotation[0])*Matrix4::makeYRotation(rotation[1])*Matrix4::makeZRotation(rotation[2]);
    mat_object = Matrix4::makeTranslation(position) * mat_object;
    mat_object = mat_object * Matrix4::makeScale(scale);
    
    mat_model_ = mat_object;
    
//    mat_model_ = mat_object;//Matrix4::makeTranslation(Cvec3(1,1,-1))* Matrix4::makeYRotation(-150);
    
    
}

void LinesSphereModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update();
}


void LinesSphereModel::Render(){
    glDisable(GL_CULL_FACE);
    
    glUseProgram(simpleShaderState_->program);
    
    glLineWidth(3.0f);

    
    glUniform3f(simpleShaderState_->lineColor_, lineColor[0],
                lineColor[1], lineColor[2]);
    
    
    glUniform1f(simpleShaderState_->lineOpacity_, 0.9f);
    
    
    Matrix4 mat_mvp = mat_projection_ * mat_view_ * mat_model_;
    
    
    GLfloat glmatrix[16];
    mat_mvp.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(simpleShaderState_->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
    glBindVertexArray(vao_);
    glDrawArrays(GL_LINES, 0,geometry_->vboLen);
    
    glBindVertexArray(0);
}

