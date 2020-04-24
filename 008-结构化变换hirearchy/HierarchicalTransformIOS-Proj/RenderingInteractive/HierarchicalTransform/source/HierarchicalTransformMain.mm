#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <sstream>

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

#include "cvec.h"
#include "matrix4.h"
#include "geometrymaker.h"
#include "glsupport.h"
#include "FileWrapper.h"

#include "quat.h"
#include "rigtform.h"
#include "arcball.h"

#include "Geometry.h"

#include "Camera.h"

#include "CubeGeometry.h"
#include "CubeModel.h"
#include "RayCaster.h"




using namespace std;      // for string, vector, iostream, and other standard C++ stuff

// G L O B A L S ///////////////////////////////////////////////////

float zTranslationDistance = -5;


static  float g_frustMinFov = 60.0;  // A minimal of 60 degree field of view
static float g_frustFovY = g_frustMinFov; // FOV in y direction (updated by updateFrustFovY)

static const float g_frustNear = -0.1;    // near plane
static const float g_frustFar = -10000.0;    // far plane

static int g_windowWidth = 512;
static int g_windowHeight = 512;
static float g_arcballScreenRadius = 0.5 * (float)min(g_windowWidth,g_windowHeight);

static bool g_mouseClickDown = false;    // is the mouse button pressed
static bool g_mouseLClickButton, g_mouseRClickButton, g_mouseMClickButton, g_quadTap;
static int g_mouseClickX, g_mouseClickY; // coordinates for mouse click event
static int g_activeShader = 0;


// --------- Scene

static const Cvec3 g_light1(2.0, 3.0, 14.0), g_light2(-2, -3.0, -5.0);  // define two lights positions in world space
                                                                        //static Matrix4 g_skyRbt = Matrix4::makeTranslation(Cvec3(0.0, 0.25, 4.0));
static RigTForm g_skyRbt = RigTForm(Cvec3(0.0, 0.25, 4.0));
//static Matrix4 g_objectRbt[2] = {Matrix4::makeTranslation(Cvec3(0,0,0)),Matrix4::makeTranslation(Cvec3(0,0,-2))};
static RigTForm g_objectRbt[2] = {RigTForm(Cvec3(0.5,0.5,0.5)),RigTForm(Cvec3(0,0,0))};

static const Cvec3 g_objectFrameOrigin = Cvec3(0,-0.25,-4.0);


///////////////// END OF G L O B A L S //////////////////////////////////////////////////
CubeModel *rootObj;
Cvec3 cameraPosition;
shared_ptr<MatrixStack> matrixStack;

//shared_ptr<TorusModel> torusModel;
//shared_ptr<CubeModel> cubeModel;
vector<shared_ptr<CubeModel>> cubes;
//shared_ptr<CubeModel> selectedCube;
CubeModel *selectedCube;
//vector<CubeModel *> cubes;
vector<Cvec3> mInstancePos;
vector<Cvec3> mInstanceOrientation;
vector<Cvec3> mInstanceScale;
vector<Cvec3> mInstanceDiffuseColor;

shared_ptr<PerspectiveCamera> mainCamera;
RayCaster rayCaster;

static float timer;

//--------------------------------------------------------------------------------
//  touch event variables
//--------------------------------------------------------------------------------
//记录touch事件当前的接触位置
GLfloat touch_location_x;
GLfloat touch_location_y;
//sphere rotation angle about cube
GLfloat rotation_angle;
GLfloat rotation_angle_alpha;
GLfloat rotation_angle_slerp_start=75.f;
GLfloat rotation_angle_slerp_end=255.f;
RigTForm g_slerpBaseRbt = RigTForm(Cvec3(0,0,-2));


// update g_frustFovY from g_frustMinFov, g_windowWidth, and g_windowHeight
static void updateFrustFovY() {
    if (g_windowWidth >= g_windowHeight)
        g_frustFovY = g_frustMinFov;
    else {
        const double RAD_PER_DEG = 0.5 * CS175_PI/180;
        g_frustFovY = atan2(sin(g_frustMinFov * RAD_PER_DEG) * g_windowHeight / g_windowWidth, cos(g_frustMinFov * RAD_PER_DEG)) / RAD_PER_DEG;
    }
}

void traverse(CubeModel *obj,Cvec3 rotation,Matrix4 viewMat){
    for(auto object:(*obj->GetChildren())){
        CubeModel *model = dynamic_cast<CubeModel *>(object);
        model->setPerspectiveCamera(mainCamera);
        model->rotation = rotation;
        model->UpdateMatrixWorld();
        model->mat_view_ = viewMat;
        
        if(model->GetChildren()->size()>0){
            traverse(model, rotation,viewMat);
        }
    }
}

static void drawStuff() {
    float touchX = ( touch_location_x - g_windowWidth ) * 10;
    float touchY = ( touch_location_y - g_windowHeight ) * 10;


    timer+=0.005;


    cameraPosition = Cvec3(cameraPosition[0]+(touchX - cameraPosition[0])*0.05,cameraPosition[1]+(- touchY - cameraPosition[1])*0.05,cameraPosition[2]);


    Matrix4 mat_eye = Matrix4::makeLookAtEyeMatrix(cameraPosition, Cvec3(0,0,0), Cvec3(0,1,0));

    Matrix4 viewMat = inv(mat_eye);

    mainCamera->eyeMat=mat_eye;

    float rx = sin(timer * 0.7)*0.2*180;
    float ry = sin(timer * 0.3)*0.1*180;
    float rz = sin(timer * 0.2)*0.1*180;

    rootObj->rotation = Cvec3(rx,ry,rz);
    rootObj->UpdateMatrixWorld();

    rootObj->setPerspectiveCamera(mainCamera);
    rootObj->mat_view_ = viewMat;

    traverse(rootObj,Cvec3(rx,ry,rz),viewMat);
    
    
    if(rootObj){
        rootObj->Render();
    }
    
}

static void display() {
    //glUseProgram(g_shaderStates[g_activeShader]->program);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);                   // clear framebuffer color&depth
    
    drawStuff();
    
    //iOS automatically control swapping buffer
    //no need to swap buffer manually
    
    checkGlError(__FUNCTION__);
}

static void reshape(const int w, const int h) {
    g_windowWidth = w;
    g_windowHeight = h;
    glViewport(0, 0, w, h);
    cerr << "Size of window is now " << w << "x" << h << endl;
    updateFrustFovY();
    
    float aspect = g_windowWidth / static_cast <double> (g_windowHeight);
    mainCamera->aspect = aspect;
    mainCamera->fov = g_frustMinFov;
    mainCamera->far = g_frustFar;
    mainCamera->near = g_frustNear;
    mainCamera->updatePorjectonMatrix();
    
    mainCamera->view=ViewPort(0, 0, w, h);
    
    for(auto cubeModel:cubes){
        cubeModel->setPerspectiveCamera(mainCamera);
        cubeModel->Init();
    }
    
}

static void motion(const float x, const float y) {
    const double dx = x - g_mouseClickX;
    const double dy = g_windowHeight - y - 1 - g_mouseClickY;
    
    //Matrix4 m;
    RigTForm m;
    if (g_mouseLClickButton && !g_mouseRClickButton) { // left button down?
                                                       //m = Matrix4::makeXRotation(-dy) * Matrix4::makeYRotation(dx);
        m = RigTForm::makeXRotation(-dy) * RigTForm::makeYRotation(dx);
    }
    else if (g_mouseRClickButton && !g_mouseLClickButton) { // right button down?
                                                            //m = Matrix4::makeTranslation(Cvec3(dx, dy, 0) * 0.01);
        m = RigTForm(Cvec3(dx, dy, 0) * 0.01);
    }
    else if (g_mouseMClickButton || (g_mouseLClickButton && g_mouseRClickButton)) {  // middle or (left and right) button down?
                                                                                     //m = Matrix4::makeTranslation(Cvec3(0, 0, -dy) * 0.01);
        m = RigTForm(Cvec3(0, 0, -dy) * 0.01);
    }
    else if (g_quadTap){
        
    }
    
    if (g_mouseClickDown) {
        //g_objectRbt[0] = g_objectRbt[0] * m; // Simply right-multiply is WRONG
        RigTForm auxiliaryRbt = makeMixedFrame(g_objectRbt[0], g_skyRbt);
        g_objectRbt[0] = doQtoOwrtA(m, g_objectRbt[0], auxiliaryRbt);
    }
    
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;
}


static void mouse(const float x, const float y,u_long tapCount,bool press) {
    g_mouseClickX = x;
    g_mouseClickY = g_windowHeight - y - 1;  // conversion from window-coordinate-system to OpenGL window-coordinate-system
    
    //every time initialze all mouse status to false
    g_mouseLClickButton=g_mouseMClickButton=g_mouseRClickButton=g_quadTap=false;
    
    g_mouseLClickButton |= (press && tapCount == 1);
    g_mouseRClickButton |= (press && tapCount == 2);
    g_mouseMClickButton |= (press && tapCount == 3);
    g_quadTap           |= (press && tapCount == 4);
    
    //    g_mouseLClickButton &= !(!press && tapCount == 1);
    //    g_mouseRClickButton &= !(!press && tapCount == 2);
    //    g_mouseMClickButton &= !(!press && tapCount == 3);
    if(!press)
        g_mouseLClickButton=g_mouseMClickButton=g_mouseRClickButton=g_quadTap=false;
    
    
    g_mouseClickDown = g_mouseLClickButton || g_mouseRClickButton || g_mouseMClickButton;
}

//static void keyboard(const unsigned char key, const int x, const int y) {
//  switch (key) {
//  case 27:
//    exit(0);                                  // ESC
//  case 'h':
//    cout << " ============== H E L P ==============\n\n"
//    << "h\t\thelp menu\n"
//    << "s\t\tsave screenshot\n"
//    << "f\t\tToggle flat shading on/off.\n"
//    << "o\t\tCycle object to edit\n"
//    << "v\t\tCycle view\n"
//    << "drag left mouse to rotate\n" << endl;
//    break;
//  case 's':
//    glFlush();
//    writePpmScreenshot(g_windowWidth, g_windowHeight, "out.ppm");
//    break;
//  case 'f':
//    g_activeShader ^= 1;
//    break;
//  }
//}


static void initGLState() {
    //设置color buffer默认颜色
    //glClearColor(128./255., 200./255., 255./255., 0.);
    glClearColor(0., 0., 0., 0.);
    //重置depth buffer默认值为0.f
    glClearDepthf(0.f);
    
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);
    //glReadBuffer(GL_BACK);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
}

static void initGeometry() {
//    cubeModel.reset(new CubeModel());
//    torusModel.reset(new MultiPassScene());
    cameraPosition = Cvec3(0,0,500);
    matrixStack.reset(new MatrixStack());
    
    mainCamera.reset(new PerspectiveCamera());
    mainCamera->updatePorjectonMatrix();
    
    //着色器状态由所有的立方体模型共享
    shared_ptr<NormalColorShaderState> shaderState;
    shaderState.reset(new NormalColorShaderState());
    
    //立方几何体对象由所有的立方体模型共享
    shared_ptr<Geometry> geometry;
    CubeGeometry cb = CubeGeometry(100,100,100);
    CubeGeometry::CubeVertexData cbVdata = makeCubeVertexData(cb);
    
    int num_vertices_ = (int)cbVdata.vData.size();
    int num_indices_ = (int)cbVdata.iData.size();
    
    vector<VertexPNX> vertices = cbVdata.vData;
    vector<unsigned short> indices = cbVdata.iData;
    geometry.reset(new Geometry(&vertices[0],&indices[0],num_vertices_,num_indices_));
    
    //每条触须的立方体模型数量
    int amount = 100;
    
    //中心根立方体模型
    rootObj = new CubeModel(NULL,geometry,shaderState);
    rootObj->position = Cvec3(1000,0,0);
    rootObj->scale = vec3(1,1,1);
    rootObj->UpdateMatrixWorld();
    rootObj->matrixStatck = matrixStack;
    
    Object *parentObj = rootObj;
    //每个循环生成一条触须，每个触须都是根立方体模型的子部件
    for(int i = 0; i < amount; i ++){
        CubeModel *model = new CubeModel(parentObj,geometry,shaderState);
        model->position = Cvec3(100,0,0);
        model->scale = Cvec3(1,1,1);
        model->matrixStatck = matrixStack;
        model->setPerspectiveCamera(mainCamera);
        parentObj = model;
    }

    parentObj = rootObj;
    for(int i = 0; i < amount; i ++){
        CubeModel *model = new CubeModel(parentObj,geometry,shaderState);
        model->position = Cvec3(-100,0,0);
        model->scale = Cvec3(1,1,1);
        model->matrixStatck = matrixStack;
        model->setPerspectiveCamera(mainCamera);
        parentObj = model;
    }

    parentObj = rootObj;
    for(int i = 0; i < amount; i ++){
        CubeModel *model = new CubeModel(parentObj,geometry,shaderState);
        model->position = Cvec3(0,-100,0);
        model->scale = Cvec3(1,1,1);
        model->matrixStatck = matrixStack;
        model->setPerspectiveCamera(mainCamera);
        parentObj = model;
    }

    parentObj = rootObj;
    for(int i = 0; i < amount; i ++){
        CubeModel *model = new CubeModel(parentObj,geometry,shaderState);
        model->position = Cvec3(0,100,0);
        model->scale = Cvec3(1,1,1);
        model->matrixStatck = matrixStack;
        model->setPerspectiveCamera(mainCamera);
        parentObj = model;
    }

    parentObj = rootObj;
    for(int i = 0; i < amount; i ++){
        CubeModel *model = new CubeModel(parentObj,geometry,shaderState);
        model->position = Cvec3(0,0,-100);
        model->scale = Cvec3(1,1,1);
        model->matrixStatck = matrixStack;
        model->setPerspectiveCamera(mainCamera);
        parentObj = model;
    }

    parentObj = rootObj;
    for(int i = 0; i < amount; i ++){
        CubeModel *model = new CubeModel(parentObj,geometry,shaderState);
        model->position = Cvec3(0,0,100);
        model->scale = Cvec3(1,1,1);
        model->matrixStatck = matrixStack;
        model->setPerspectiveCamera(mainCamera);
        parentObj = model;
    }

}

bool GraphicsInit()
{
    //printOpenGLESInfo();
    initGLState();
    //initShaders();
    initGeometry();
    
    return true;
}

bool GraphicsResize( int width, int height )
{
    reshape(width, height);
    double radiusSFactor = getScreenToEyeScale(-4, g_frustFovY, g_windowHeight);
    g_arcballScreenRadius = 2 / radiusSFactor;
    
    return true;
}

//--------------------------------------------------------------------------------
//  redraw scene per frame
//--------------------------------------------------------------------------------
bool GraphicsRender() {
    try {
        display();
        motion(touch_location_x, touch_location_y);
        
        return 0;
    }
    catch (const runtime_error& e) {
        cout << "Exception caught: " << e.what() << endl;
        return -1;
    }
}

bool comp(const IntersectionData &a, const IntersectionData &b){
    return a.distance < b.distance;
};

void TouchEventDown( float x, float y,unsigned long tapCount,bool pressStatus )
{
    touch_location_x = x;
    touch_location_y = y;
    
    mouse(x,y,tapCount,pressStatus);
    
    rayCaster.setFromCamera(Cvec3(x,y,0), mainCamera);
    //注意：此处intersecs数据需要根据距离进行排序，最近距离也就是最先被射线命中的物体才应该被选中
    vector<IntersectionData> intersects = rayCaster.intersectObjects(cubes);

    //sort intersects according to the distance member of IntersectionData increasingly
    sort(intersects.begin(),intersects.end(),comp);
    
//    for(IntersectionData interData:intersects){
//        CubeModel* cm = interData.object;
//
//        cm->diffuseColor = hexStringToRGB("0xff0000");
//        cm->position = Cvec3(0,0,0);
//
//    }
    if(intersects.size()>0 && selectedCube!=intersects[0].object){
        selectedCube = intersects[0].object;
    }else{
        selectedCube = nullptr;
    }
    
    
}

void TouchEventMove( float x, float y,unsigned long touchCount )
{
    touch_location_x = x;
    touch_location_y = y;
    
    //--------------------------------------------------------------------------------
    // arcball interface applying
    //--------------------------------------------------------------------------------
    if(touchCount > 1){
        
    }
}

void TouchEventRelease( float x, float y,unsigned long tapCount,bool pressStatus )
{
    //todo
    if(tapCount == 4){
        g_activeShader ^= 1; //g_activeShader变量和1相同则设置为0，不同则设置为1
    }
    //--------------------------------------------------------------------------------
    //  after touch moving event，touch up event doesn't have correcsponding touch down event tapCount
    //--------------------------------------------------------------------------------
    
    mouse(x,y,tapCount,pressStatus);
}
