//--------------------------------------------------------------------------------
// CubeModel.cpp
// Render a plane
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
// Include files
//--------------------------------------------------------------------------------
#include "CubeModel.h"

#include "matrix4.h"
#include "Geometry.h"

#include "CubeGeometry.h"

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#include <cmath>
#include <sstream>
#include "RayCaster.h"


float CubeModel::timer=0.f;

CubeModel::CubeModel(std::shared_ptr<Geometry> geometry,std::shared_ptr<PhongShaderState> phongShaderState):geometry_(geometry),phongShaderState_(phongShaderState) {
    vao_ = 0;
    vbo_ = 0;
    ibo_ = 0;
    
    // Load shader
    //phongShaderState_.reset(new PhongShaderState());
    
//    CubeGeometry cb = CubeGeometry();
//    CubeGeometry::CubeVertexData cbVdata = makeCubeVertexData(cb);
//    
//    // Create VBO
//    num_vertices_ = (int)cbVdata.vData.size();
//    num_indices_ = (int)cbVdata.iData.size();
//    
//    vector<VertexPNX> vertices = cbVdata.vData;
//    vector<unsigned short> indices = cbVdata.iData;
//    geometry_.reset(new Geometry(&vertices[0],&indices[0],num_vertices_,num_indices_));
    num_indices_ = geometry_->iboLen;
    
    if(!vao_)
        glGenVertexArrays(1, &vao_);
    
    glBindVertexArray(vao_);
    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
    
    int32_t iStride = sizeof(VertexPNX);
    // Pass the vertex data
    glVertexAttribPointer(TEXTURE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(0));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_VERTEX);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_NORMAL);
    
    glVertexAttribPointer(TEXTURE_ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
                          BUFFER_OFFSET(2* 3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(TEXTURE_ATTRIB_UV);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_->ibo);
    
    glBindVertexArray(0);
    
}

CubeModel::~CubeModel(){
    Unload();
}

void CubeModel::Init() {
    
//    mat_model_ = Matrix4::makeTranslation(Cvec3(0.f, 0.f, 0.f));
    
    //mat_model_ =  mat_model_ * Matrix4::makeXRotation(45);
    
//    mat_view_ = Matrix4::makeTranslation(Cvec3(0.0f,0.0f,20.0f));
//    mat_view_ = inv(mat_view_);
    
    int count = 50;
    float scale = 5.0;
    
    for (int i = 0; i < count; i ++ ) {
        
        double r = (rand()%100)/100.0 * 2.0 * M_PI;
        double z = ((rand()%100)/100.0 * 2.0 ) - 1.0;
        double zScale = sqrt( 1.0 - z * z ) * scale;
        
        mInstancePos.push_back(Cvec3(cos(r) * zScale,sin( r ) * zScale,-z*scale));
        mInstanceOrientation.push_back(Cvec3((rand()%100)/100.0* 2.0 * M_PI,(rand()%100)/100.0* 2.0 * M_PI,(rand()%100)/100.0* 2.0 * M_PI));
        
    }

}

void CubeModel::Unload() {
    if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    
    if (ibo_) {
        glDeleteBuffers(1, &ibo_);
        ibo_ = 0;
    }
    
}

void CubeModel::Update(double time) {
    mat_projection_ = perspectiveCamera_->projMat;
    
}

void CubeModel::UpdateMatrixWorld() {
    Matrix4 mat_object = Matrix4::makeXRotation(rotation[0])*Matrix4::makeYRotation(rotation[1])*Matrix4::makeZRotation(rotation[2]);
    mat_object = Matrix4::makeTranslation(position) * mat_object;
    mat_object = mat_object * Matrix4::makeScale(scale);
    
    mat_model_ = mat_object;
    
//    mat_model_ = mat_object;//Matrix4::makeTranslation(Cvec3(1,1,-1))* Matrix4::makeYRotation(-150);
    
    
}

void CubeModel::setPerspectiveCamera(std::shared_ptr<PerspectiveCamera> camera){
    this->perspectiveCamera_ = camera;
    
    Update(0);
}

void CubeModel::RenderMultiInsances(){
        
//    for (int i = 0; i < mInstancePos.size(); i ++ ) {
//        
//        mat_model_ = Matrix4::makeTranslation(mInstancePos[i]);
//        mat_model_ = mat_model_ * Matrix4::makeXRotation(mInstanceOrientation[i][0]) * Matrix4::makeYRotation(mInstanceOrientation[i][0]) * Matrix4::makeZRotation(mInstanceOrientation[i][0]);
//        
//        mat_model_ = mat_model_ * Matrix4::makeTranslation(Cvec3(0,0,zTranslationDistance)) * Matrix4::makeYRotation(-250);
//            
//            Render();
//        }
//    Render();
}

void CubeModel::Render(){
    glDisable(GL_CULL_FACE);
    
    glUseProgram(phongShaderState_->program);

    
    TORUS_MATERIALS material = {
        {1.0, 0.055, 0.027}, {1.0f, 1.0f, 1.0f, 90.f}, {0.1f, 0.1f, 0.1f}, };
    
   
    //diffuseColor = diffuseColor * (rand()%100/(100.0));
    // Update uniforms
    glUniform4f(phongShaderState_->material_diffuse_, diffuseColor[0],
                diffuseColor[1], diffuseColor[2], 1.f);
    
    glUniform4f(phongShaderState_->material_specular_, material.specular_color[0],
                material.specular_color[1], material.specular_color[2],
                material.specular_color[3]);
    
    glUniform3f(phongShaderState_->material_ambient_, material.ambient_color[0],
                material.ambient_color[1], material.ambient_color[2]);
    
    glUniform3f(phongShaderState_->light0_, -2.f, 5.f, 50.f);
    
    //mat_model_ = mat_model_ * Matrix4::makeYRotation(0.0625);
    //mat_model_ = mat_model_ * Matrix4::makeXRotation(0.25);
    // Feed Projection and Model View matrices to the shaders
    //Matrix4 mat_vp = mat_projection_ * mat_view_ * mat_model_;
    
    Matrix4 mat_mv = mat_view_ * mat_model_;
    
    
    GLfloat glmatrix[16];
    mat_mv.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(phongShaderState_->matrix_mv_, 1, GL_FALSE,
                       glmatrix);
    
    GLfloat glmatrix2[16];
    mat_projection_.writeToColumnMajorMatrix(glmatrix2);
    glUniformMatrix4fv(phongShaderState_->matrix_p_, 1, GL_FALSE,
                       glmatrix2);
    
    
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, num_indices_,GL_UNSIGNED_SHORT,NULL);
    
    glBindVertexArray(0);
}

//执行射线投射动作，使用raster发出射线，然后计算相交。
//这个方法是raycast的核心，每个对象都会调用
vector<IntersectionData> CubeModel::rayCast(RayCaster *rayCaster){
    if(!isPickingEnabled) return{};
    
    vector<vec3> intersectPoints;
    
    Matrix4 matrixWorld = mat_model_;
    

    this->boundingSphere = new Sphere();
    this->boundingSphere->setFromPoints(geometry_->points);

    //将BV副本转换为world coordinate
    Sphere *sphere = new Sphere();
    sphere->center = boundingSphere->center;
    sphere->radius = boundingSphere->radius;

    sphere->applyMatrix4(matrixWorld);

    // 先进行了ray-sphere相交检测
    Ray *ray = rayCaster->ray;
    intersectPoints = ray->intersectSphere(sphere);
    if (intersectPoints.size()==0) return {};

//    IntersectionData idata=intersectPoints[0];
////    IntersectionData idata={{}};
//    idata.object = this;
    
    
    //Ray *ray = rayCaster->ray;
    //先计算object matrix的反转，用于将射线转换为object coordinate
    Matrix4 inverseMatrix = inv(matrixWorld);
    Ray *rayObj = new Ray();
    rayObj->origin = ray->origin;
    rayObj->direction = ray->direction;

    rayObj->applyMatrix4(inverseMatrix);

    // Check boundingBox before continuing
    // 计算ray-triangle前先检查ray-box相交作保守型检测，用于优化
    Box* boundingBox;
    boundingBox = new Box();
    boundingBox->setFromPoints(geometry_->points);
    //    if(this->boundingBox == NULL){
//            this->boundingBox = new Box();
//            this->boundingBox->setFromPoints(points);
    //    }
//    vector<Cvec3> intersectData = rayObj->intersectBox(boundingBox);
//    if (intersectData.size()==0) return {};
    
    intersectPoints = rayObj->intersectBox(boundingBox);
    if (intersectPoints.size()==0) return {};
    
    intersectPoints.clear();
    for (int i = 0, il = geometry_->iboLen; i < il; i += 3 ) {
        
        Cvec3 a = geometry_->points[geometry_->indice[i]];
        Cvec3 b = geometry_->points[geometry_->indice[i+1]];
        Cvec3 c = geometry_->points[geometry_->indice[i+2]];
        
        vector<Cvec3> intersectData =  rayObj->intersectTriangle(a, b, c, true);
        
        for(auto point:intersectData){
            intersectPoints.push_back(point);
        }
        
    }
    
    if (intersectPoints.size()==0) return {};
    if (intersectPoints.size()>0){
        vector<IntersectionData> idata;
        
        for(auto point:intersectPoints){
            IntersectionData interObj={};
            interObj.intersectionPointWorld = Cvec3(matrixWorld * Cvec4(point,1.0));
            interObj.object = this;
            interObj.distance = norm(point-rayCaster->ray->origin);
            
            idata.push_back(interObj);
        }
        
        return idata;
        
    }

//    IntersectionData idata={};//intersectPoints[0];
//    //    IntersectionData idata={{}};
//    idata.intersectionPointWorld = intersectData[0];
//    idata.object = this;
//
    return {};
}

