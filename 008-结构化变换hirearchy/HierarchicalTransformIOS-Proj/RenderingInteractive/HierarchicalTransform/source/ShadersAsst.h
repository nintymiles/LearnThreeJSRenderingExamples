#ifndef ASSTCOMMON_H
#define ASSTCOMMON_H

#include <cstddef>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cmath>

#include "glsupport.h"
#include "matrix4.h"
#include "FileWrapper.h"

struct NormalColorShaderState{
    
    GlProgram program;
    
    GLuint matrix_mv_;
    GLuint matrix_p_;
    
    NormalColorShaderState() {
        static std::string vsfn = "NormalColorShaderVert.glsl";
        static std::string fsfn = "NormalColorShaderFrag.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mv_ = safe_glGetUniformLocation(program, "uModelViewMatrix");
        matrix_p_ = safe_glGetUniformLocation(program, "uProjectionMatrix");
        
        checkGlError(__FUNCTION__);
    }
    
};


struct PhongShaderState{
    
    GlProgram program;
    
    GLuint light0_;
    GLuint material_diffuse_;
    GLuint material_ambient_;
    GLuint material_specular_;
    
    GLuint matrix_p_;
    GLuint matrix_mv_;
    
    GLuint userClipPlane_;
    
    PhongShaderState() {
        static std::string vsfn =  "PhongShaderVert.glsl";
        static std::string fsfn =  "PhongShaderFrag.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_p_ = safe_glGetUniformLocation(program, "uProjectionMatrix");
        matrix_mv_ = safe_glGetUniformLocation(program, "uModelViewMatrix");
        
        light0_ = safe_glGetUniformLocation(program, "vLight0");
        material_diffuse_ = safe_glGetUniformLocation(program, "vMaterialDiffuse");
        material_ambient_ = safe_glGetUniformLocation(program, "vMaterialAmbient");
        material_specular_ =
        safe_glGetUniformLocation(program, "vMaterialSpecular");
        
        //userClipPlane_ = safe_glGetUniformLocation(program,"uUserClipPlane");
        
        checkGlError(__FUNCTION__);
    }
    
};


#endif
