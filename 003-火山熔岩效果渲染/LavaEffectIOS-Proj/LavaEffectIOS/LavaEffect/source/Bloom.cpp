//
//  Bloom.cpp
//
//  Created by macbook on 12/20/14.
//  Copyright (c) 2014 macbook. All rights reserved.
//

#include "Bloom.h"
#define NIL -1

int TEXTURE_WIDTH = 1536;
int TEXTURE_HEIGHT = 2480;

unsigned int Bloom::generateTexture(int width,int height,bool isDepth)
{
	unsigned int texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (isDepth){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        
    }
    else{
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }

	int error;
	error=glGetError();
	if(error != 0)
	{
		std::cout << "Error: Fail to generate texture." << error << std::endl;
	}
	glBindTexture(GL_TEXTURE_2D,0);
	return texId;
}

void Bloom::GenerateSceneFBO()
{
    float tWidth = TEXTURE_WIDTH ;
    float tHeight = TEXTURE_HEIGHT ;
    
    // create a framebuffer object
    glGenFramebuffers(1, &SceneFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, SceneFbo);
    
    //Create color and depth buffer texture object
    SceneTexture = generateTexture(tWidth,tHeight);
    DepthTexture = generateTexture(tWidth,tHeight, true);
    
    // create a renderbuffer object to store depth info
//    GLuint rboId;
//    glGenRenderbuffers(1, &rboId);
//    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, TEXTURE_WIDTH,TEXTURE_HEIGHT);

    
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER,        // 1. fbo target: GL_FRAMEBUFFER
                           GL_COLOR_ATTACHMENT0,  // 2. Color attachment point
                           GL_TEXTURE_2D,         // 3. tex target: GL_TEXTURE_2D
                           SceneTexture,          // 4. color texture ID
                           0);                    // 5. mipmap level: 0(base)
    
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER,        // 1. fbo target: GL_FRAMEBUFFER
                           GL_DEPTH_ATTACHMENT,   // 2. Depth attachment point
                           GL_TEXTURE_2D,         // 3. tex target: GL_TEXTURE_2D
                           DepthTexture,          // 4. depth texture ID
                           0);                    // 5. mipmap level: 0(base)
    
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer creation fails: %d", status);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
}

void Bloom::GenerateBloomFBO()
{
    float tWidth = TEXTURE_WIDTH ;
    float tHeight = TEXTURE_HEIGHT ;
   
    // create a framebuffer object
    glGenFramebuffers(1, &BloomFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, BloomFbo);
    
    //Create color and depth buffer texture object
    BloomTexture = generateTexture(tWidth,tHeight);
    
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER,        // 1. fbo target: GL_FRAMEBUFFER
                           GL_COLOR_ATTACHMENT0,  // 2. Color attachment point
                           GL_TEXTURE_2D,         // 3. tex target: GL_TEXTURE_2D
                           BloomTexture,            // 4. color texture ID
                           0);                    // 5. mipmap level: 0(base)
    
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer creation fails: %d", status);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Bloom::GenerateBlurFBO()
{
    float tWidth = TEXTURE_WIDTH ;
    float tHeight = TEXTURE_HEIGHT ;
    
    // create a framebuffer object
    glGenFramebuffers(1, &BlurFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, BlurFbo);
    
    //Create color and depth buffer texture object
    BlurTexture = generateTexture(tWidth,tHeight);
    
    // attach the texture to FBO color attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER,        // 1. fbo target: GL_FRAMEBUFFER
                           GL_COLOR_ATTACHMENT0,  // 2. Color attachment point
                           GL_TEXTURE_2D,         // 3. tex target: GL_TEXTURE_2D
                           BlurTexture,          // 4. color texture ID
                           0);                    // 5. mipmap level: 0(base)
    
    // check FBO status
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE){
        printf("Framebuffer creation fails: %d", status);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Bloom::Bloom()
{
    
    torusModel.reset(new TorusModel());
    textureQuad.reset(new MultiPassTexture());
    
    //Initialize the Polka dots
    torusModel->Init();
    
    //Initialize the Simple Texture dots
    textureQuad->InitModel();
    
}

Bloom::~Bloom()
{
    glDeleteFramebuffers(1, &SceneFbo);
    glDeleteRenderbuffers(1, &BlurFbo);
    
    glDeleteTextures(1, &SceneTexture);
    SceneTexture = 0;

    glDeleteTextures(1, &BloomTexture);
    BloomTexture = 0;

    glDeleteTextures(1, &DepthTexture);
    DepthTexture = 0;
}

void Bloom::InitModel()
{
    
    ClearTextures();
    
    // Generate the Frame buffer object
    GenerateSceneFBO();
    GenerateBloomFBO();
    GenerateBlurFBO();
}

void Bloom::RenderBloom(GLint BindTexture, GLint framebuffer, GLint ColorTexture, GLint DepthBuf)
{
    // Bind Framebuffer 2
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture, 0);
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, BindTexture);
    
    textureQuad->ApplyShader(BloomShader);
    textureQuad->Render();
}

void Bloom::RenderSpecFBO(GLint BindTexture, GLint framebuffer, GLint ColorTexture, GLint DepthBuf)
{
    // Bind Framebuffer 2
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture, 0);
    textureQuad->ApplyShader(SimpleShader);
    
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, BindTexture);
    
    glActiveTexture (GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ColorTexture);
    
    
    textureQuad->Render();
}


void Bloom::RenderObj(GLint BindTexture,GLint framebuffer, GLint ColorTexture, GLint DepthBuf){
    
    // Bind Framebuffer 1
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuf, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    torusModel->setPerspectiveCamera(perspectiveCamera_);
    torusModel->Render();
}

void Bloom::Render()
{
    // Get current Framebuffer
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &DefaultFrameBuffer);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    RenderObj(NIL,SceneFbo,SceneTexture,DepthTexture);
    
//    SetUpOrthoProjection();
    //glClear(GL_COLOR_BUFFER_BIT);
//    glDisable(GL_DEPTH_TEST);
    textureQuad->imageIncrement[0] = 0.001953125;
    textureQuad->imageIncrement[1] = 0.0;
    RenderBloom(SceneTexture,BloomFbo,BloomTexture,NIL);
//    glDisable(GL_DEPTH_TEST);
    textureQuad->imageIncrement[0] = 0.0;
    textureQuad->imageIncrement[1] = 0.001953125;
    RenderBloom(BloomTexture,BlurFbo,BlurTexture,NIL);
    

    RenderSpecFBO(SceneTexture,DefaultFrameBuffer,BlurTexture,NIL);
    
//    RenderHorizontalBlur(BloomTexture,BlurFbo,BlurTexture,NIL);
//    glDisable(GL_DEPTH_TEST);
//    RenderVerticalBlur(NIL,DefaultFrameBuffer,NIL,NIL);
}

void Bloom::ClearTextures(){
    GLuint textures[] = {SceneTexture, BlurTexture, BloomTexture, DepthTexture};
    GLuint fbos[] = {SceneFbo, BlurFbo, BloomFbo};
    if (textures) {
        glDeleteTextures(4, textures);
        SceneTexture = BlurTexture = BloomTexture = DepthTexture = 0;
    }
    
    if (fbos) {
        glDeleteFramebuffers(3, fbos);
        SceneFbo = BlurFbo = BloomFbo = 0;
    }
}

void Bloom::TouchEventDown( float x, float y )
{
//    objModel->TouchEventDown( x, y );
//    textureQuad->TouchEventDown( x, y );
}

void Bloom::TouchEventMove( float x, float y )
{
//    textureQuad->TouchEventMove( x, y );
}

void Bloom::TouchEventRelease( float x, float y )
{
    //textureQuad->TouchEventRelease( x, y );
}


void Bloom::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    TEXTURE_WIDTH = perspectiveCamera_->view.width;
    TEXTURE_HEIGHT = perspectiveCamera_->view.height;
    
}
