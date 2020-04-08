//
//  MultiPassScene.h
//

#ifndef __MultiPassScene__
#define __MultiPassScene__
#include "MultiPassTexture.h"
#include "TorusKnotModel.h"
#include "Camera.h"

#include <iostream>



class MultiPassScene{
private:
    
    shared_ptr<TorusKnotModel> torusKnotModel;
    shared_ptr<MultiPassTexture> textureQuad;
    GLuint SceneTexture, BlurTexture, MultiPassSceneTexture, DepthTexture;
    
    
    
    int DefaultFrameBuffer;
    
    GLuint SceneFbo, BlurFbo, MultiPassSceneFbo;
    
    std::shared_ptr<PerspectiveCamera> perspectiveCamera_;
    
public:
    MultiPassScene();
    ~MultiPassScene();
    
    void InitModel();
    // Framebuffer related functions.
    unsigned int generateTexture(int width,int height,bool isDepth=false);
    void GenerateSceneFBO();
    void ClearTextures();
    
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    
    // Render
    void Render();
    void RenderObj(GLint BindTexture, GLint Framebuffer, GLint ColorBuf, GLint DepthBuf);
    
    void RenderSpecFBO(GLint BindTexture, GLint framebuffer, GLint ColorTexture, GLint DepthBuf);
    
//    // ProjectionSystem
//    void SetUpPerspectiveProjection();
//    void SetUpOrthoProjection();
    
    // TouchEvents
    void TouchEventDown( float x, float y );
    void TouchEventMove( float x, float y );
    void TouchEventRelease( float x, float y );
    
};

#endif /* defined(__MultiPassScene__) */
