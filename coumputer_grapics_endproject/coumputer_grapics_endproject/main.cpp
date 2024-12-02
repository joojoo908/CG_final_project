#include <iostream>
#include <random>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>  // GLFW 대신 GLUT 헤더를 포함

#pragma comment(lib, "glew32.lib")
#include <gl/glm/glm.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Model.h"
#include "Animation.h"
#include "Animator.h"
#include "FrameBuffer.h"
#include "FreeCamera.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Player.h"
#include "PlayerCamera.h"
#include "Skybox.h"

#include "ShaderHandle.h"
#include "Object.h"

#define WIDTH 1280
#define HEIGHT 720

//int screenWidth = GetSystemMetrics(SM_CXSCREEN);
//int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int screenWidth = WIDTH;
int screenHeight = HEIGHT;
int Center_width = screenWidth / 2;
int Center_height = screenHeight / 2;
int lastX, lastY;
bool fixed_center{};
bool key_f1 = 1;

// Global variables
Window* mainWindow;
//Camera* camera;
CameraBase* currCamera;
FreeCamera* freeCamera;
PlayerCamera* playerCamera;


GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

std::vector<Mesh*> meshList;
std::vector<Entity*> entityList;


//Model* model_2B;
Model* mainModel;
Model* cube;
Model* ground;
Model* currModel;

Player* player;
std::vector<Object*> objs;
Object* object;
Object* object2;

Animator* animator;
Animator* noani;

Animation* idleAnim;
Animation* danceAnim;
Animation* runAnim;

DirectionalLight* directionalLight;
PointLight* pointLights[MAX_POINT_LIGHTS];
unsigned int pointLightCount = 0;

Skybox* skybox;

extern GLuint loc_modelMat;
extern GLuint loc_PVM;
extern GLuint loc_diffuseSampler;
extern GLuint loc_normalSampler;
extern GLuint loc_normalMat;
extern GLuint loc_eyePos;
extern GLuint loc_finalBonesMatrices;
// Keyboard and mouse control

void TimerFunction(int value);

//키보드 눌림 함수
void processKeyboard(unsigned char key, int x, int y) {
    if (key_f1) {
        currCamera->KeyControl(key, deltaTime);
    }
    else {
        player->HandleInput(key, 1, deltaTime);
    }
    if (key == '1') {
        if (animator->GetCurrAnimation() != idleAnim)
            animator->PlayAnimation(idleAnim);
    }
    if (key == '2') {
        if (animator->GetCurrAnimation() != danceAnim)
            animator->PlayAnimation(danceAnim);
    }
    if (key == '3') {
        if (animator->GetCurrAnimation() != runAnim)
            animator->PlayAnimation(runAnim);
    }
    if (key == '0')
    {
        lastX = Center_width;
        lastY = Center_height;
        fixed_center = !fixed_center;
    }
}
//키보드 떼어짐 함수
void processKeyboardUp(unsigned char key, int x, int y) {
    player->HandleInput(key, 0, deltaTime);
    if (key == 'w') {

    }
}

void SpecialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
        key_f1 = !key_f1;
        break;
    case GLUT_KEY_F2:
        key_f1 = 0;
        currCamera = playerCamera;
        break;
    case GLUT_KEY_F3:
        key_f1 = 1;
        currCamera = freeCamera;
        break;
    }
}

//안씀
void MoveCamera()
{
    //currCamera->KeyControl(mainWindow->getsKeys(), deltaTime);
    //currCamera->ScrollControl(mainWindow->GetScrollYChange());
}

bool click = 0;
void processMouse(int x, int y) {
    GLfloat XChange = x - lastX;
    GLfloat YChange = lastY - y;
    currCamera->MouseControl(XChange, YChange);
    player->MouseContrl(XChange, YChange);

    lastX = x;
    lastY = y;

    currCamera->Update();
    if (fixed_center)
    {
        lastX = Center_width;
        lastY = Center_height;
        glutWarpPointer(Center_width, Center_height);
    }
}

void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        click = 1;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        click = 0;
    }
}
void Motion(int x, int y) {
    GLfloat XChange = x - Center_width;
    GLfloat YChange = Center_height - y;
    currCamera->MouseControl(XChange, YChange);
    player->MouseContrl(XChange, YChange);

    lastX = x;
    lastY = y;

    currCamera->Update();
    if (fixed_center)
    {
        lastX = Center_width;
        lastY = Center_height;
        glutWarpPointer(Center_width, Center_height);
    }
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
}

//--------------------------------

void update() {

    player->update(deltaTime);
    currCamera->Update();
}

void mainInit() {
    CreateShader();
    CreateShader_obj();

    //빛
    directionalLight = new DirectionalLight(0.5f, 1.f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(0.f, 1.f, 1.f));
    entityList.push_back(directionalLight);

    pointLights[0] = new PointLight
    (0.f, 0.5f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(0.f, 1.5f, 0.2f),
        1.0f, 0.22f, 0.20f);
    pointLightCount++;
    pointLights[1] = new PointLight
    (0.0f, 0.5f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(-2.0f, 2.0f, -1.f),
        1.0, 0.045f, 0.0075f);
    pointLightCount++;
    for (int i = 0; i < pointLightCount; i++)
        entityList.push_back(pointLights[i]);

    // Skybox
    std::vector<std::string> skyboxFaces;
    skyboxFaces.push_back("Skybox/px.png");
    skyboxFaces.push_back("Skybox/nx.png");
    skyboxFaces.push_back("Skybox/py.png");
    skyboxFaces.push_back("Skybox/ny.png");
    skyboxFaces.push_back("Skybox/pz.png");
    skyboxFaces.push_back("Skybox/nz.png");
    skybox = new Skybox(skyboxFaces);


    //모델 추가용 init() 함수 추가 예정
    // Model loading
    mainModel = new Model();
    std::string modelPath = "Knight/test.gltf";
    //std::string modelPath = "obj/night.gltf";
    mainModel->LoadModel(modelPath);
    entityList.push_back(mainModel);

    //모델 90도 회전
    GLfloat* currRot = mainModel->GetRotate();
    float rotation = 90;
    float newRotx = currRot[0] + rotation;
    glm::vec3 newRot(newRotx, currRot[1], currRot[2]);
    mainModel->SetRotate(newRot);

    //----------------------------------------
    std::random_device rd;  // 하드웨어 난수 생성기
    std::mt19937 gen(rd());  // Mersenne Twister 엔진
    std::uniform_int_distribution<> dis(-100, 100);

    cube = new Model();
    modelPath = "obj/tree.gltf";
    cube->LoadModel(modelPath);
    for (int i = 0; i < 10; i++) {
        object = new Object(cube, 0, dis(gen), dis(gen));
        objs.push_back(object);
    }
    
    /*object = new Object(modelPath,0,0,0);
    object2 = new Object(modelPath,0,1,1);*/

    //----------------------------------------
    ground = new Model();
    modelPath = "Ground3/gnd_v0.gltf";
    ground->LoadModel(modelPath);
    
    currRot = ground->GetRotate();
    rotation = 90;
    newRotx = currRot[0] + rotation;
    glm::vec3 newRot3(newRotx, currRot[1], currRot[2]);
    ground->SetRotate(newRot3);

    glm::vec3 newTns3(0,0,0);
    ground->SetTranslate(newTns3);
    glm::vec3 newscale(100, 1, 100);
    ground->SetScale(newscale);





    animator = new Animator(nullptr);
    noani = new Animator(nullptr);

    //플레이어 연결
    player = new Player(mainModel, animator);

    freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 2.f), 100.f, 0.3f);
    playerCamera = new PlayerCamera(player);
    currCamera = freeCamera;


    /*idleAnim = new Animation("Knight/idle.gltf", currModel);
    danceAnim = new Animation("Knight/dance.gltf", currModel);
    runAnim = new Animation("Knight/run.gltf", currModel);*/

}

GLvoid render()
{

    update();

    GLfloat now = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    deltaTime = now - lastTime;
    lastTime = now;
    //std::cout << "deltaTime"<< deltaTime << std::endl;

    //--------------

    // Clear the window
    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //무언가 바인드

    glm::mat4 viewMat = currCamera->GetViewMatrix();
    glm::mat4 projMat = currCamera->GetProjectionMatrix(screenWidth, screenHeight);
    glm::vec3 camPos = currCamera->GetPosition();

    skybox->DrawSkybox(viewMat, projMat);

    /*
    terrain->UseShader();
    terrain->GetShader()->UseDirectionalLight(directionalLight);
    terrain->GetShader()->UsePointLights(pointLights, pointLightCount);
    terrain->GetShader()->UseEyePos(camPos);
    terrain->DrawTerrain(viewMat, projMat);*/

    //전체가 플레이어 그리기

    //나무 그리기
   /* shaderList[1]->UseShader();
    {
        GetShaderHandles_obj();

        glm::mat4 modelMat = cube->GetModelMat();
        glm::mat4 PVM = projMat * viewMat * modelMat;
        glm::mat3 normalMat = GetNormalMat(modelMat);

        glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
        glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        shaderList[1]->UseEyePos(camPos);
        shaderList[1]->UseDirectionalLight(directionalLight);
        shaderList[1]->UsePointLights(pointLights, pointLightCount);

        shaderList[1]->UseMaterial(cube->GetMaterial());

        glUniform1i(loc_diffuseSampler, 0);
        glUniform1i(loc_normalSampler, 1);

        cube->RenderModel();
        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cout << "error : " << error << std::endl;
        }
    }*/
    for (auto object : objs) {
        object->draw(currCamera, directionalLight, pointLights, pointLightCount);
    }

    //object->draw(currCamera, directionalLight, pointLights, pointLightCount);
    //object2->draw(currCamera, directionalLight, pointLights, pointLightCount);
    //땅 그리기
    shaderList[1]->UseShader();
    {
        GetShaderHandles_obj();

        glm::mat4 modelMat = ground->GetModelMat();
        glm::mat4 PVM = projMat * viewMat * modelMat;
        glm::mat3 normalMat = GetNormalMat(modelMat);

        glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
        glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

        shaderList[1]->UseEyePos(camPos);
        shaderList[1]->UseDirectionalLight(directionalLight);
        shaderList[1]->UsePointLights(pointLights, pointLightCount);

        shaderList[1]->UseMaterial(ground->GetMaterial());

        glUniform1i(loc_diffuseSampler, 0);
        glUniform1i(loc_normalSampler, 1);

        ground->RenderModel();
        glBindTexture(GL_TEXTURE_2D, 0);

        GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            std::cout << "error : " << error << std::endl;
        }
    }

    player->draw(currCamera, directionalLight, pointLights, pointLightCount);


    glutSwapBuffers();  // Swap buffers to render
}

int main(int argc, char** argv)
{
    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("OpenGL with Assimp and GLUT");


    // 전체 화면으로 전환
    //glutFullScreen();

    // Initialize GLEW
    glewInit();
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);



    mainInit();

    // Set callbacks
    glutDisplayFunc(render);
    glutReshapeFunc(handleResize);
    glutSpecialFunc(SpecialKeyboard);
    glutMouseFunc(Mouse);
    glutKeyboardFunc(processKeyboard);
    glutKeyboardUpFunc(processKeyboardUp);
    glutPassiveMotionFunc(processMouse);
    glutMotionFunc(Motion);
    //glutTimerFunc(1, TimerFunction, 1);
    glutIdleFunc(render);  // Continuously render when idle

    // Start GLUT main loop
    glutMainLoop();

    return 0;
}

void TimerFunction(int value) {
    //glutPostRedisplay(); // 화면 재 출력
    glutTimerFunc(1, TimerFunction, 1);
}