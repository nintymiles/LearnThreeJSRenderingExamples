//
//  Bloom.h
//
//
//  Created by macbook on 12/20/14.
//  Copyright (c) 2014 macbook. All rights reserved.
//

#ifndef __Bloom__
#define __Bloom__
#include "MultiPassTexture.h"
#include "TorusModel.h"
#include "Camera.h"

#include <iostream>



class Bloom{
private:
    
    shared_ptr<TorusModel> torusModel;
    shared_ptr<MultiPassTexture> textureQuad;
    GLuint SceneTexture, BlurTexture, BloomTexture, DepthTexture;
    
    
    
    int DefaultFrameBuffer;
    
    GLuint SceneFbo, BlurFbo, BloomFbo;
    
    std::shared_ptr<PerspectiveCamera> perspectiveCamera_;
    
public:
    Bloom();
    ~Bloom();
    
    void InitModel();
    // Framebuffer related functions.
    unsigned int generateTexture(int width,int height,bool isDepth=false);
    void GenerateSceneFBO();
    void GenerateBloomFBO();
    void GenerateBlurFBO();
    void ClearTextures();
    
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    
    // Render
    void Render();
    void RenderObj(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    void RenderBloom(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    void RenderHorizontalBlur(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    void RenderVerticalBlur(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    
    void RenderSpecFBO(GLint BindTexture, GLint framebuffer, GLint ColorTexture, GLint DepthBuf);
    
//    // ProjectionSystem
//    void SetUpPerspectiveProjection();
//    void SetUpOrthoProjection();
    
    // TouchEvents
    void TouchEventDown( float x, float y );
    void TouchEventMove( float x, float y );
    void TouchEventRelease( float x, float y );
    
};

#endif /* defined(__Bloom__) */
