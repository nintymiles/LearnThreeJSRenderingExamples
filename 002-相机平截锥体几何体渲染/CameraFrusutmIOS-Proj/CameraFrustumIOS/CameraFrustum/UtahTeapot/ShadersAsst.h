#ifndef ASSTCOMMON_H
#define ASSTCOMMON_H

#include <cstddef>
#include <vector>
#include <memory>
#include <stdexcept>

#include "glsupport.h"
#include "matrix4.h"
#include "FileWrapper.h"

struct SimpleTextureShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    
    SimpleTextureShaderState() {
        static std::string vsfn = "SimpleTextureVertex.glsl";
        static std::string fsfn = "SimpleTextureFragment.glsl";
        
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        
        checkGlError(__FUNCTION__);
    }
    
};

struct TeapotShaderState{
    
    GlProgram program;
    
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;
    
    GLuint matrix_projection_;
    GLuint matrix_view_;
    
    TeapotShaderState() {
        static std::string vsfn =  "PhongShader.vsh";
        static std::string fsfn =  "PhongShader.fsh";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_projection_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        matrix_view_ = safe_glGetUniformLocation(program, "uMVMatrix");
        
        light0_ = safe_glGetUniformLocation(program, "vLight0");
        material_diffuse_ = safe_glGetUniformLocation(program, "vMaterialDiffuse");
        material_ambient_ = safe_glGetUniformLocation(program, "vMaterialAmbient");
        material_specular_ =
        safe_glGetUniformLocation(program, "vMaterialSpecular");
        
        checkGlError(__FUNCTION__);
    }
    
};

struct FrustumShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    
    FrustumShaderState() {
        static std::string vsfn = "SimpleVertex.glsl";
        static std::string fsfn = "SimpleFragment.glsl";
        
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uMVPMatrix");
        
        checkGlError(__FUNCTION__);
    }
    
};


#endif
