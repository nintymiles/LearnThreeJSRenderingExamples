#ifndef __MultiPassTexture__
#define __MultiPassTexture__

#include <iostream>

#include "ShadersAsst.h"
#include "matrix4.h"
#include "Camera.h"
#include "Geometry.h"

using namespace std;

enum ShaderType{
    SimpleShader = 0,
    BloomShader,
    HorizontalBlurShader,
    VerticalBlurShader
};

class MultiPassTexture
{
    
public:
    // Constructor for ObjLoader
    MultiPassTexture();
    
    // Destructor for ObjLoader
    ~MultiPassTexture();
    
    // Initialize our Model class
    void InitModel();
    
    // Render the Model class
    void Render();
    
    //std::shared_ptr<LavaShaderState> lavaShader;
    
    Matrix4 mat_model_;
    Matrix4 mat_projection_;
    Matrix4 mat_view_;
    float imageIncrement[2]={0,0};
    
    // Touch Events
	void TouchEventDown( float x, float y );
	void TouchEventMove( float x, float y );
	void TouchEventRelease( float x, float y );
    
    void Update(double time);
    void setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera);
    
    void ApplyShader(ShaderType shader);
    
private:
    GLuint vao_;
    int32_t num_vertices_;
    
    std::shared_ptr<Geometry> geometry_;
    
    std::shared_ptr<DepthTextureShaderState> depthTextureShaderState;
    
    ShaderType shaderType;
    

};


#endif /* defined(__MultiPassTexture__) */
