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
static const float g_frustFar = -500.0;    // far plane

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

static void drawStuff() {
    
    timer+=0.1;
    float radius = 10;
    float eyeX = radius * sin(timer*M_PI/180);
    float eyeY = radius * sin(timer*M_PI/180);
    float eyeZ = radius * cos(timer*M_PI/180);
    Matrix4 mat_eye = Matrix4::makeLookAtEyeMatrix(Cvec3(eyeX,eyeY,eyeZ), Cvec3(0,0,0), Cvec3(0,1,0));
//    Matrix4 mat_eye = Matrix4::makeTranslation(Cvec3(0,0,2));
    
    Matrix4 viewMat = inv(mat_eye);
    
    mainCamera->eyeMat=mat_eye;
    
    

//    cubeModel->Render();
//    cubeModel->RenderMultiInsances();
    int i=0;
    for(auto cubeModel:cubes){
        if(cubeModel.get() == selectedCube)
            cubeModel->diffuseColor = hexStringToRGB("0x00ff00");
        else
            cubeModel->diffuseColor = mInstanceDiffuseColor[i];

        cubeModel->mat_view_ = viewMat;

//
//        cubeModel->position = mInstancePos[i];
//        cubeModel->rotation = mInstanceOrientation[i];
//        cubeModel->scale = mInstanceScale[i];
//        cubeModel->diffuseColor = hexStringToRGB("0xffffff");//mInstanceDiffuseColor[i];
//        cubeModel->UpdateMatrixWorld();
        cubeModel->Render();
        i++;
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
    
    shared_ptr<PhongShaderState> phongShaderState;
    phongShaderState.reset(new PhongShaderState());
    
    shared_ptr<Geometry> geometry;
    CubeGeometry cb = CubeGeometry(1,1,1,2,2,2);
    CubeGeometry::CubeVertexData cbVdata = makeCubeVertexData(cb);
    
    // Create VBO
    int num_vertices_ = (int)cbVdata.vData.size();
    int num_indices_ = (int)cbVdata.iData.size();
    
    vector<VertexPNX> vertices = cbVdata.vData;
    vector<unsigned short> indices = cbVdata.iData;
    geometry.reset(new Geometry(&vertices[0],&indices[0],num_vertices_,num_indices_));
    
    int count = 1000;
    
    for (int i = 0; i < count; i ++ ) {
        
        //srand((int)time(0));
        double x = rand()%50 - 25;//(rand()%100)/100.0 * 2.0 * M_PI;
        double y = rand()%50 - 25;
        double z = rand()%50 - 25;
        mInstancePos.push_back(Cvec3(x,y,z));
        
        mInstanceOrientation.push_back(Cvec3(rand()%360,rand()%360,rand()%360));
        
        mInstanceScale.push_back(Cvec3((rand()%100)/100.0+0.5,(rand()%100)/100.0+0.5,
                                       (rand()%100)/100.0+0.5));
//        Matrix4 mat_model_ = Matrix4::makeTranslation(Cvec3(cos(r) * zScale,sin( r ) * zScale,-z*scale));
//        mat_model_ = mat_model_ * Matrix4::makeXRotation((rand()%100)/100.0* 2.0 * M_PI) * Matrix4::makeYRotation((rand()%100)/100.0* 2.0 * M_PI) * Matrix4::makeZRotation((rand()%100)/100.0* 2.0 * M_PI);
        
        //random diffuse color
        int cInt = 0xffffff * (rand()%100/(100.0));
        std::stringstream stream;
        stream << std::hex << cInt;
        std::string hexString( stream.str() );
        Cvec3 diffuseColor = hexStringToRGB(hexString);
        mInstanceDiffuseColor.push_back(diffuseColor);
        
        shared_ptr<CubeModel> cubeModel=make_shared<CubeModel>(CubeModel(geometry,phongShaderState));
        //CubeModel *cubeModel = new CubeModel(geometry,phongShaderState);
        //cubeModel->mat_model_=mat_model_;
        //cubeModel->diffuseColor = diffuseColor;
        
//        Matrix4 mat_model = Matrix4::makeTranslation(mInstancePos[i]);
//        mat_model = mat_model * Matrix4::makeXRotation(mInstanceOrientation[i][0]) * Matrix4::makeYRotation(mInstanceOrientation[i][1]) * Matrix4::makeZRotation(mInstanceOrientation[i][2]);
        
//        mat_model = mat_model * Matrix4::makeTranslation(Cvec3(0,0,zTranslationDistance));
        
        cubeModel->position = mInstancePos[i];
        cubeModel->rotation = mInstanceOrientation[i];
        cubeModel->scale = mInstanceScale[i];
        cubeModel->diffuseColor = mInstanceDiffuseColor[i]; //hexStringToRGB("0xffffff");//
        cubeModel->UpdateMatrixWorld();
        
        cubes.push_back(cubeModel);
        
    }
    
    
    mainCamera.reset(new PerspectiveCamera());
    mainCamera->updatePorjectonMatrix();

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
        
//        Cvec2 startScreenPos = Cvec2(g_mouseClickX,g_mouseClickY);
//        Cvec2 endScreenPos = Cvec2(x,g_windowHeight - y - 1); //convert from window coordnate to OpenGL window coordinate.
//        Cvec2 centerScreenPos = getScreenSpaceCoord(g_objectFrameOrigin,makeProjectionMatrix(), 0.0, 0.0, g_windowWidth, g_windowHeight);
//        Quat arcballQuat = arcball(Cvec3(centerScreenPos,0), g_arcballScreenRadius, startScreenPos, endScreenPos);
//        //ego motion
//        //g_skyRbt = g_skyRbt * RigTForm(arcballQuat);
//        //让camera围绕sphere转动，以保持sphere一直在视野中心,around the auxiliary frame
//        g_skyRbt = doQtoOwrtA(RigTForm(arcballQuat), g_skyRbt, g_objectRbt[1]);
        
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
