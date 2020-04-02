#include "MultiPassTexture.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>


#define VERTEX_POSITION 0
#define VERTEX_NORMAL 1
#define TEX_COORD 2

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

float texCoords[8] ={   0.0f, 0.0f,
                        1.0f, 0.0f,
                        0.0f, 1.0f,
                        1.0f, 1.0f
                    };

float quad[12]     ={
                        -1.0f, -1.0f,  0.0f,
                         1.0f, -1.0f,  0.0f,
                        -1.0f, 1.0f, -0.0f,
                         1.0f, 1.0f, -0.0f
                    };

// Namespace used
using std::ifstream;
using std::ostringstream;

MultiPassTexture::MultiPassTexture():vao_(0){
    
    //bloomShaderState.reset(new BloomShaderState());
    bloomShaderState.reset(new ConvolutionShaderState());
    composeTexturesShaderState.reset(new ComposeTexturesShaderState());
    
    num_vertices_ = sizeof(quad) / sizeof(quad[0]) /3;
    
    int32_t index = 0;
    int32_t tIndex = 0;
    VertexPNX* p = new VertexPNX[num_vertices_];
    for (int32_t i = 0; i < num_vertices_; ++i) {
        p[i].p[0] = quad[index];
        p[i].p[1] = quad[index + 1];
        p[i].p[2] = quad[index + 2];
        
        p[i].n[0] = 0;
        p[i].n[1] = 0;
        p[i].n[2] = 0;
        index += 3;
        
        p[i].x[0] = texCoords[tIndex];
        p[i].x[1] = texCoords[tIndex + 1];
        
        tIndex += 2;
    }
    
    geometry_.reset(new Geometry(p,NULL,num_vertices_,0));
    
    delete[] p;
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPNX);
    // Pass the vertex data
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(VERTEX_POSITION);
    
//    glVertexAttribPointer(VERTEX_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
//                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(VERTEX_NORMAL);
    
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(2* 3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEX_COORD);
    
    glBindVertexArray(0);
}


MultiPassTexture::~MultiPassTexture()
{

}


void MultiPassTexture::InitModel()
{
    
    
    
//    mat_view_ = Matrix4::makeTranslation(Cvec3(0,0,4.0f));
//    mat_view_ = inv(mat_view_);
    
    float span=1.0f;
    mat_projection_ = Matrix4::makeOrthoProjection(-span, span, -span, span, -span, span);
    
//    mat_projection_ = Matrix4::makeProjection(30, 1, -0.1, -1000.f);
    
}

void MultiPassTexture::Render()
{
    glDisable(GL_CULL_FACE);

    Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    
    
    
    
    GLfloat glmatrix[16];
    mat_vp.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(bloomShaderState->matrix_mvp_, 1, GL_FALSE,
                       glmatrix);
    
    if(shaderType==BloomShader)
        glUniform2fv(bloomShaderState->imageIncrement_,1,imageIncrement);
    
    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void MultiPassTexture::TouchEventDown( float x, float y ){

}

void MultiPassTexture::TouchEventMove( float x, float y ){

}

void MultiPassTexture::TouchEventRelease( float x, float y ){

}

void MultiPassTexture::ApplyShader(ShaderType shader){
    GLuint program;
    
    if (shader == HorizontalBlurShader){
        program =-1;
    }
    else if (shader == VerticalBlurShader){
        program =-1;
    }
    else if (shader == BloomShader){
        program = bloomShaderState->program;
    }
    else{
        program = composeTexturesShaderState->program;
    }
    
    shaderType = shader;
    glUseProgram(program);
}

