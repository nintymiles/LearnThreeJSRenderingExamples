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

struct ComposeTexturesShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    GLuint tex1_;
    GLuint tex2_;
    
    ComposeTexturesShaderState() {
        static std::string vsfn = "ComposeTexturesShaderVert.glsl";
        static std::string fsfn = "ComposeTexturesShaderFrag.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        tex1_ = safe_glGetUniformLocation(program, "tDiffuse");
        tex2_ = safe_glGetUniformLocation(program, "effectTex");
        
        glUseProgram(program);
        glUniform1i(tex1_, 0);
        glUniform1i(tex2_, 1);
        
        checkGlError(__FUNCTION__);
    }
    
};

struct ConvolutionShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    GLuint tex1_;
    GLuint imageIncrement_;
    GLuint gaussianWeight_;
    
    float gaussEquation(float val, float sigma){
        double coefficient = 1.0/(2.0*M_PI*sigma);
        double exponent    = -(val*val)/(2.0*sigma);
        return (float) (coefficient*exp(exponent));
    }
    
    //优化方法
    float gauss(float val,float sigma){
        return exp(-(val*val)/(2.0 * sigma * sigma));
    }
    
    std::vector<float> buildKernal(float sigma){
        float sum, halfWidth;
        int kMaxKernelSize = 25, kernelSize = 2 * ceil(sigma * 3.0) + 1;
        
        if(kernelSize > kMaxKernelSize)
            kernelSize = kMaxKernelSize;
        
        halfWidth = (kernelSize - 1) * 0.5;
        
        std::vector<float> values(kernelSize);
        
        sum = 0.0;
        for(int i = 0; i < kernelSize; ++ i){
            values[i] = gauss(i - halfWidth, sigma);
            sum += values[i];
        }
        
        // normalize the kernel
        for (int i = 0; i < kernelSize; ++ i)
            values[i] /= sum;
        
        return values;
    }
    
    ConvolutionShaderState() {
        static std::string vsfn = "ConvolutionShaderVert.glsl";
        static std::string fsfn = "ConvolutionShaderFrag.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uModelViewProjectionMatrix");
        tex1_ = safe_glGetUniformLocation(program, "tDiffuse");
        imageIncrement_ = safe_glGetUniformLocation(program,"uImageIncrement");
        gaussianWeight_ = safe_glGetUniformLocation(program,"cKernel[0]");
        

        
        glUseProgram(program);
        glUniform1i(tex1_, 0);
        
        float sigma=5.0;
        std::vector<float> gaussKernals = buildKernal(sigma);
        glUniform1fv(gaussianWeight_, (int)gaussKernals.size(), &gaussKernals[0]);
        
        checkGlError(__FUNCTION__);
    }
    
};

struct LavaShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_,matrix_mv_,matrix_p_;
    GLuint utime_,fogDensity_,fogColor_,uvScale_,tex1_,tex2_;
    GLuint userClipPlane_;
    
    LavaShaderState() {
        static std::string vsfn = "LavaShaderVert.glsl";
        static std::string fsfn = "LavaShaderFrag.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uModelViewProjectionMatrix");
        matrix_mv_ = safe_glGetUniformLocation(program, "uModelViewMatrix");
        matrix_p_ = safe_glGetUniformLocation(program, "uProjectionMatrix");
        
        utime_ = safe_glGetUniformLocation(program, "time");
        fogDensity_ = safe_glGetUniformLocation(program, "fogDensity");
        fogColor_ = safe_glGetUniformLocation(program, "fogColor");
        uvScale_ = safe_glGetUniformLocation(program, "uvScale");
        
        tex1_ = safe_glGetUniformLocation(program, "texture1");
        tex2_ = safe_glGetUniformLocation(program, "texture2");
        
        userClipPlane_ = safe_glGetUniformLocation(program,"uUserClipPlane");
        
        
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
        
        userClipPlane_ = safe_glGetUniformLocation(program,"uUserClipPlane");
        
        checkGlError(__FUNCTION__);
    }
    
};


#endif
