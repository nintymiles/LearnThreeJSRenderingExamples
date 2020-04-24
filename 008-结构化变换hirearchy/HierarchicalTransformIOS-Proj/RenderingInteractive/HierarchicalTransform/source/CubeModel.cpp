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

CubeModel::CubeModel(Object *parent,std::shared_ptr<Geometry> geometry,std::shared_ptr<NormalColorShaderState> shaderState):geometry_(geometry),shaderState_(shaderState),Model(parent) {
    vao_ = 0;
    vbo_ = 0;
    ibo_ = 0;
    
    // Load shader
    //shaderState_.reset(new PhongShaderState());
    
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

    
//    num_indices_ = geometry_->iboLen;
//
//    if(!vao_)
//        glGenVertexArrays(1, &vao_);
//
//    glBindVertexArray(vao_);
//    // Bind the VBO
//    glBindBuffer(GL_ARRAY_BUFFER, geometry_->vbo);
//
//    int32_t iStride = sizeof(VertexPNX);
//    // Pass the vertex data
//    glVertexAttribPointer(TEXTURE_ATTRIB_VERTEX, 3, GL_FLOAT, GL_FALSE, iStride,
//                          BUFFER_OFFSET(0));
//    glEnableVertexAttribArray(TEXTURE_ATTRIB_VERTEX);
//
//    glVertexAttribPointer(TEXTURE_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, iStride,
//                          BUFFER_OFFSET(3 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(TEXTURE_ATTRIB_NORMAL);
//
//    glVertexAttribPointer(TEXTURE_ATTRIB_UV, 2, GL_FLOAT, GL_FALSE, iStride,
//                          BUFFER_OFFSET(2* 3 * sizeof(GLfloat)));
//    glEnableVertexAttribArray(TEXTURE_ATTRIB_UV);
//
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry_->ibo);
//
//    glBindVertexArray(0);
    
}

CubeModel::~CubeModel(){
    Unload();
}

void CubeModel::Init() {
    
//    mat_model_ = Matrix4::makeTranslation(Cvec3(0.f, 0.f, 0.f));
    
    //mat_model_ =  mat_model_ * Matrix4::makeXRotation(45);
    
//    mat_view_ = Matrix4::makeTranslation(Cvec3(0.0f,0.0f,20.0f));
//    mat_view_ = inv(mat_view_);
    
//    int count = 50;
//    float scale = 5.0;
//
//    for (int i = 0; i < count; i ++ ) {
//
//        double r = (rand()%100)/100.0 * 2.0 * M_PI;
//        double z = ((rand()%100)/100.0 * 2.0 ) - 1.0;
//        double zScale = sqrt( 1.0 - z * z ) * scale;
//
//        mInstancePos.push_back(Cvec3(cos(r) * zScale,sin( r ) * zScale,-z*scale));
//        mInstanceOrientation.push_back(Cvec3((rand()%100)/100.0* 2.0 * M_PI,(rand()%100)/100.0* 2.0 * M_PI,(rand()%100)/100.0* 2.0 * M_PI));
//
//    }

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

void CubeModel::Update() {
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
    
    Update();
}

void CubeModel::ApplyModelsTransformation(){
    matrixStatck->MatrixStackMultiplyMatrix(&mat_model_);
}


void CubeModel::Render(){
    glDisable(GL_CULL_FACE);
    
    glUseProgram(shaderState_->program);
    
    //首先压栈
    matrixStatck->MatrixStackPushMatrix();
    //将压栈后的顶部矩阵元素串接当前运动参数矩阵
    matrixStatck->MatrixStackMultiplyMatrix(&mat_model_);
    //矩阵栈的使用
    Matrix4 mat_mv = mat_view_ * (*matrixStatck->MatrixStackGetTopMatrix());
    
    GLfloat glmatrix[16];
    mat_mv.writeToColumnMajorMatrix(glmatrix);
    glUniformMatrix4fv(shaderState_->matrix_mv_, 1, GL_FALSE,
                       glmatrix);
    
    GLfloat glmatrix2[16];
    mat_projection_.writeToColumnMajorMatrix(glmatrix2);
    glUniformMatrix4fv(shaderState_->matrix_p_, 1, GL_FALSE,
                       glmatrix2);
    
    geometry_->draw(*shaderState_);
    
    for(auto object:*this->GetChildren()){
        CubeModel *model = dynamic_cast<CubeModel *>(object);
        model->Render();
    }
    //出栈
    matrixStatck->MatrixStackPopMatrix();
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
    
    //计算object matrix的反转，用于将射线转换为object coordinate
    Matrix4 inverseMatrix = inv(matrixWorld);
    Ray *rayObj = new Ray();
    rayObj->origin = ray->origin;
    rayObj->direction = ray->direction;
    //将屏幕射线转化为物体坐标
    rayObj->applyMatrix4(inverseMatrix);
    
    Box* boundingBox;
    boundingBox = new Box();
    boundingBox->setFromPoints(geometry_->points);
    
    // ray-box检测，相对ray-sphere更准确一些，用于二次检测
    intersectPoints = rayObj->intersectBox(boundingBox);
    if (intersectPoints.size()==0) return {};
    
    intersectPoints.clear();
    
    // 如果两次保守型检测仍然不能排除相交，则执行开销大的ray-triangle检测，用于获得精确的结果
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
    // 生成ray-geometry相交数据
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

    return {};
}

