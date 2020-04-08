//
//  MultiPassScene.cpp
//

#include "MultiPassScene.h"
#define NIL -1

int TEXTURE_WIDTH = 1536;
int TEXTURE_HEIGHT = 2480;

unsigned int MultiPassScene::generateTexture(int width,int height,bool isDepth)
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

void MultiPassScene::GenerateSceneFBO()
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

MultiPassScene::MultiPassScene()
{
    
    torusKnotModel.reset(new TorusKnotModel());
    textureQuad.reset(new MultiPassTexture());
    
    //Initialize the Polka dots
    torusKnotModel->Init();
    
    //Initialize the Simple Texture dots
    textureQuad->InitModel();
    
}

MultiPassScene::~MultiPassScene()
{
    glDeleteFramebuffers(1, &SceneFbo);
    glDeleteRenderbuffers(1, &BlurFbo);
    
    glDeleteTextures(1, &SceneTexture);
    SceneTexture = 0;

    glDeleteTextures(1, &MultiPassSceneTexture);
    MultiPassSceneTexture = 0;

    glDeleteTextures(1, &DepthTexture);
    DepthTexture = 0;
}

void MultiPassScene::InitModel()
{
    
    ClearTextures();
    
    // Generate the Frame buffer object
    GenerateSceneFBO();
}

void MultiPassScene::RenderSpecFBO(GLint BindTexture, GLint framebuffer, GLint ColorTexture, GLint DepthBuf)
{
    // Bind Framebuffer 2
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    textureQuad->ApplyShader(SimpleShader);
    
    
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ColorTexture);
    
    glActiveTexture (GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,BindTexture);
    
    
    textureQuad->Render();
}


void MultiPassScene::RenderObj(GLint BindTexture,GLint framebuffer, GLint ColorTexture, GLint DepthBuf){
    
    // Bind Framebuffer 1
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffer);
    glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture,0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthBuf, 0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    torusKnotModel->setPerspectiveCamera(perspectiveCamera_);
    torusKnotModel->RenderMultiInsances();
}

void MultiPassScene::Render()
{
    // Get current Framebuffer
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &DefaultFrameBuffer);

    glEnable(GL_DEPTH_TEST);
    RenderObj(NIL,SceneFbo,SceneTexture,DepthTexture);
    
    RenderSpecFBO(DepthTexture,DefaultFrameBuffer,SceneTexture,NIL);
    
}

void MultiPassScene::ClearTextures(){
    GLuint textures[] = {SceneTexture, BlurTexture, MultiPassSceneTexture, DepthTexture};
    GLuint fbos[] = {SceneFbo, BlurFbo, MultiPassSceneFbo};
    if (textures) {
        glDeleteTextures(4, textures);
        SceneTexture = BlurTexture = MultiPassSceneTexture = DepthTexture = 0;
    }
    
    if (fbos) {
        glDeleteFramebuffers(3, fbos);
        SceneFbo = BlurFbo = MultiPassSceneFbo = 0;
    }
}

void MultiPassScene::TouchEventDown( float x, float y )
{
//    objModel->TouchEventDown( x, y );
//    textureQuad->TouchEventDown( x, y );
}

void MultiPassScene::TouchEventMove( float x, float y )
{
//    textureQuad->TouchEventMove( x, y );
}

void MultiPassScene::TouchEventRelease( float x, float y )
{
    //textureQuad->TouchEventRelease( x, y );
}


void MultiPassScene::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    TEXTURE_WIDTH = perspectiveCamera_->view.width;
    TEXTURE_HEIGHT = perspectiveCamera_->view.height;
    
}
