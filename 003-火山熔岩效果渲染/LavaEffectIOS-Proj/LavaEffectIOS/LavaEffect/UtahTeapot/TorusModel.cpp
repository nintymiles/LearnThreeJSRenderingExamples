//--------------------------------------------------------------------------------
// TorusModel.cpp
// Render a plane
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "TorusModel.h"

#include "matrix4.h"
#include "Geometry.h"

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

float TorusModel::timer=0.f;

TorusModel::TorusModel() {
    vao_ = 0;
    vbo_ = 0;
    ibo_ = 0;
    
    // Load shader
    lavaShaderState_.reset(new LavaShaderState());
    
    std::vector<VertexPNX> vertices = loadObj("torus.obj");
    
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
    glVertexAttribPointer(TEXTURE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_VERTEX);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_NORMAL);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(2* 3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_UV);
    
    glBindVertexArray(0);
}

TorusModel::~TorusModel(){
    Unload();
}

void TorusModel::Init() {
    
    
    mat_model_ = Matrix4::makeTranslation(Cvec3(0.f, 0.f, 0.f));
    
    mat_model_ =  mat_model_ * Matrix4::makeXRotation(45);
    
    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
    mat_view_ = inv(mat_view_);
    
    
    std::vector<std::string> textures = {std::string(GetBundleFileName("cloud.png"))};
    texObj_ = Texture::Create(GL_TEXTURE_2D, textures);
    assert(texObj_);
    
//    texObj_->Activate(GL_TEXTURE2);
//    glUniform1i(lavaShaderState_->tex1_, 2);
    
    std::vector<std::string> textures1 = {std::string(GetBundleFileName("lavatile.jpg"))};
    texObj1_ = Texture::Create(GL_TEXTURE_2D, textures1);
    assert(texObj1_);
    
    
//    texObj1_->Activate(GL_TEXTURE3);
//    glUniform1i(lavaShaderState_->tex2_, 3);

}

void TorusModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
    if (ibo_) {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }
    
}

void TorusModel::Update(double time) {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void TorusModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update(0);
}

void TorusModel::Render(){
    
    glUseProgram(lavaShaderState_->program);
    
    glUniform1f(lavaShaderState_->utime_, 1.0f);
    glUniform3f(lavaShaderState_->fogColor_,0,0,0);
    glUniform1f(lavaShaderState_->fogDensity_, 0.45);
    
    glUniform2f(lavaShaderState_->uvScale_, 6.0, 2.0);
    
    texObj_->Activate(GL_TEXTURE0);
    glUniform1i(lavaShaderState_->tex1_, 0);
    
    texObj1_->Activate(GL_TEXTURE1);
    glUniform1i(lavaShaderState_->tex2_, 1);
    
    timer+=0.025f;
    if(timer>100.f)
        timer=1.0f;
    
    glUniform1f(lavaShaderState_->utime_, timer);
    
    mat_model_ = mat_model_ * Matrix4::makeYRotation(0.125);
    mat_model_ = mat_model_ * Matrix4::makeXRotation(0.5);
    // Feed Projection and Model View matrices to the shaders
    Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    
    
    GLfloat glmatrix[16];
    mat_vp.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(lavaShaderState_->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0,num_vertices_);
    
    glBindVertexArray(0);
}

