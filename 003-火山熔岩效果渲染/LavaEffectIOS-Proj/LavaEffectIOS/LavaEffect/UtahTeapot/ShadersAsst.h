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

// Change the filter size in the BlurHorizontal.glsl and BlurVertical.glsl
#define FILTER_SIZE 5
#define PIXEL_INTERVAL 2.0

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

struct HorizontalBlurShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    GLuint tex1_;
    GLuint gaussianWeightHor_;
    GLuint pixelOffsetHor_;
    GLuint screenCoordXHor_;
    GLuint pixelSize_;
    
    float gaussEquation(float val, float sigma)
    {
    double coefficient = 1.0/(2.0*M_PI*sigma);
    double exponent    = -(val*val)/(2.0*sigma);
    return (float) (coefficient*exp(exponent));
    }
    
    HorizontalBlurShaderState() {
        static std::string vsfn = "Vertex.glsl";
        static std::string fsfn = "BlurHorizontal.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "MODELVIEWPROJECTIONMATRIX");
        
        tex1_ = safe_glGetUniformLocation(program, "Tex1");
        gaussianWeightHor_ = safe_glGetUniformLocation(program,"Weight[0]");
        pixelOffsetHor_ = safe_glGetUniformLocation(program,"PixOffset[0]");
        screenCoordXHor_ = safe_glGetUniformLocation(program,"ScreenCoordX");
        pixelSize_ = safe_glGetUniformLocation(program,"pixelSize");

        glUniform1i(tex1_, 0);
        glUniform2f(pixelSize_, 1.0/256, 1.0/256);
        
        float gWeight[FILTER_SIZE];
        float sigma = 10.0;
        float sum   = 0;
        gWeight[0] = gaussEquation(0, sigma);
        sum = gWeight[0];
        for(int i = 1; i<FILTER_SIZE; i++){
            gWeight[i] = gaussEquation(i, sigma);
            sum += 2*gWeight[i];
        }
        
        for(int i = 0; i<FILTER_SIZE; i++){
            gWeight[i] = gWeight[i] / sum;
        }
        
        if (gaussianWeightHor_ >= 0){
            glUniform1fv(gaussianWeightHor_, sizeof(gWeight)/sizeof(float), gWeight);
        }
        
        float pixOffset[FILTER_SIZE];
        for(int i = 0; i<FILTER_SIZE; i++){
            pixOffset[i] = float(i)*PIXEL_INTERVAL;
        }
        
        if (pixelOffsetHor_ >= 0){
            glUniform1fv(pixelOffsetHor_, sizeof(pixOffset)/sizeof(float), pixOffset);
        }
        
        
        checkGlError(__FUNCTION__);
    }
    
};

struct VerticalBlurShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    GLuint tex1_;
    GLuint sceneTex_;
    GLuint pixelOffsetVert_;
    GLuint gaussianWeightVert_;
    GLuint screenCoordXVert_;
    GLuint pixelSize_;
    
    float gaussEquation(float val, float sigma)
    {
    double coefficient = 1.0/(2.0*M_PI*sigma);
    double exponent    = -(val*val)/(2.0*sigma);
    return (float) (coefficient*exp(exponent));
    }
    
    VerticalBlurShaderState() {
        static std::string vsfn = "Vertex.glsl";
        static std::string fsfn = "BlurVertical.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "MODELVIEWPROJECTIONMATRIX");
        
        tex1_ = safe_glGetUniformLocation(program, "Tex1");
        
        sceneTex_ = safe_glGetUniformLocation(program,"RenderTex" );
        pixelOffsetVert_ = safe_glGetUniformLocation(program,"PixOffset[0]");
        gaussianWeightVert_ = safe_glGetUniformLocation(program,"Weight[0]");
        screenCoordXVert_ = safe_glGetUniformLocation(program,"ScreenCoordX");
        pixelSize_ = safe_glGetUniformLocation(program,"pixelSize");
        
        glUniform1i(sceneTex_, 1);
        glUniform1i(tex1_, 0);
        
        float gWeight[FILTER_SIZE];
        float sigma = 10.0;
        float sum   = 0;
        
        gWeight[0]  = gaussEquation(0, sigma);
        sum = gWeight[0];
        for(int i = 1; i<FILTER_SIZE; i++){
            gWeight[i] = gaussEquation(i, sigma);
            sum += 2*gWeight[i];
        }
        
        for(int i = 0; i<FILTER_SIZE; i++){
            gWeight[i] = gWeight[i] / sum;
        }
        
        if (gaussianWeightVert_ >= 0){
            glUniform1fv(gaussianWeightVert_, sizeof(gWeight)/sizeof(float), gWeight);
        }
        
        float pixOffset[FILTER_SIZE];
        for(int i = 0; i<FILTER_SIZE; i++){
            pixOffset[i] = float(i)*PIXEL_INTERVAL;
        }
        
        if (pixelOffsetVert_ >= 0){
            glUniform1fv(pixelOffsetVert_, sizeof(pixOffset)/sizeof(float), pixOffset);
        }
        
        
        checkGlError(__FUNCTION__);
    }
    
};


struct BloomShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    GLuint tex1_;
    
    BloomShaderState() {
        static std::string vsfn = "Vertex.glsl";
        static std::string fsfn = "Bloom.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "MODELVIEWPROJECTIONMATRIX");
        
        tex1_ = safe_glGetUniformLocation(program, "Tex1");
        
        glUniform1i(tex1_, 0);
        
        
        checkGlError(__FUNCTION__);
    }
    
};

//struct BloomShaderState{
//    
//    GlProgram program;
//    
//    GLuint matrix_mvp_;
//    GLuint tex1_;
//    
//    LavaShaderState() {
//        static std::string vsfn = "Vertex.glsl";
//        static std::string fsfn = "Bloom.glsl";
//        
//        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
//        
//        // Retrieve handles to uniform variables
//        matrix_mvp_ = safe_glGetUniformLocation(program, "MODELVIEWPROJECTIONMATRIX");
//        
//        tex1_ = safe_glGetUniformLocation(program, "Tex1");
//        
//        
//        checkGlError(__FUNCTION__);
//    }
//    
//};

struct LavaShaderState{
    
    GlProgram program;
    
    GLuint matrix_mvp_;
    GLuint utime_,fogDensity_,fogColor_,uvScale_,tex1_,tex2_;
    
    LavaShaderState() {
        static std::string vsfn = "LavaShaderVert.glsl";
        static std::string fsfn = "LavaShaderFrag.glsl";
        
        readAndCompileShader(program, GetBundleFileName(vsfn.c_str()), GetBundleFileName(fsfn.c_str()));
        
        // Retrieve handles to uniform variables
        matrix_mvp_ = safe_glGetUniformLocation(program, "uModelViewProjectionMatrix");
//        matrix_mv_ = safe_glGetUniformLocation(program, "uModelViewMatrix");
//        matrix_p_ = safe_glGetUniformLocation(program, "uProjectionMatrix");
        
        utime_ = safe_glGetUniformLocation(program, "time");
        fogDensity_ = safe_glGetUniformLocation(program, "fogDensity");
        fogColor_ = safe_glGetUniformLocation(program, "fogColor");
        uvScale_ = safe_glGetUniformLocation(program, "uvScale");
        
        tex1_ = safe_glGetUniformLocation(program, "texture1");
        tex2_ = safe_glGetUniformLocation(program, "texture2");
        
        
        checkGlError(__FUNCTION__);
    }
    
};

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
