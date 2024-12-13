#include <iostream>
#include <random>
#include <vector>
#include <map>
#include<string>
#include <GL/glew.h>
#include <GL/freeglut.h>  // GLFW ´ë½Å GLUT Çì´õ¸¦ Æ÷ÇÔ

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
#include "Boss.h"

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

std::string mode = "Title_mode";
std::string back_mode = "";
bool key_f1 = 1;

// Global variables
Window* mainWindow;
//Camera* camera;
CameraBase* currCamera;
FreeCamera* freeCamera;
FreeCamera* eventCamera;
PlayerCamera* playerCamera;

GLfloat deltaTime = 0.f;
GLfloat lastTime = 0.f;

std::vector<Mesh*> meshList;
std::vector<Entity*> entityList;

Model* mainModel;
Model* boss_model;
Model* cube;
Model* machine;
Model* collide_box;
Model* ground;
Model* currModel;

Player* player;
Boss* boss;
std::vector<Object*> objs;
std::map<std::pair<int, int>, Object*> obj_map;
Object* object;
Object* title;

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

//Å°º¸µå ´­¸² ÇÔ¼ö
void processKeyboard(unsigned char key, int x, int y) {
    if (mode == "Master_mode") {
        currCamera->KeyControl(key, deltaTime);
    }
    else if (mode == "Play_mode") {
        player->HandleInput(key, 1, deltaTime);
    }

    if (key == 'p') {
        if (mode == "Pause_mode") {
            lastX = Center_width;
            lastY = Center_height;
            mode = "Play_mode";
        }
        else {
            if(mode == "Play_mode") mode = "Pause_mode";
        }
    }
    else if (key == 'm') {
        if (mode == "Master_mode") {
            mode = back_mode;
            back_mode = "";
            currCamera = playerCamera;
        }
        else {
            back_mode = mode;
            mode = "Master_mode";
            currCamera = freeCamera;
        }
    }
    
    /*if (key == '1') {
        std::cout << pointLights[0]->GetModelMat()[3][0] << std::endl;
        std::cout << pointLights[0]->GetModelMat()[3][1] << std::endl;
        std::cout << pointLights[0]->GetModelMat()[3][2] << std::endl;
    }
    if (key == '2') {
        pointLights[0]->position = glm::vec3(pointLights[0]->position[0]+1, 1.0f, pointLights[0]->position[2]);
    }*/
}
//Å°º¸µå ¶¼¾îÁü ÇÔ¼ö
void processKeyboardUp(unsigned char key, int x, int y) {
    if (mode == "Play_mode") {
        player->HandleInput(key, 0, deltaTime);
    }
}
//½ºÆä¼È Å° ÇÔ¼ö
void SpecialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
       
        break;
    case GLUT_KEY_F2:
       
        break;
    case GLUT_KEY_F3:
        
        break;
    }
}



void processMouse(int x, int y) {
    GLfloat XChange = x - lastX;
    GLfloat YChange = lastY - y;
    if ( mode != "Play_mode" && mode!="Master_mode" ) {
        XChange = 0;
        YChange = 0;
    }
    lastX = x;
    lastY = y;

    currCamera->MouseControl(XChange, YChange);

    if (mode == "Play_mode") {
        player->MouseContrl(XChange, YChange);
        lastX = Center_width;
        lastY = Center_height;
        glutWarpPointer(Center_width, Center_height);
    }

}

void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (mode == "Title_mode") {
            mode = "Play_mode";
            currCamera= playerCamera;
        }
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        
    }
}
void Motion(int x, int y) {
    /*GLfloat XChange = x - lastX;
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
    }*/
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
}
//--------------------------------

void update() {
    if (mode == "Play_mode" || back_mode == "Play_mode") {
        for (const auto& obj : obj_map) {
            obj.second->update(deltaTime, currCamera->GetPosition());
        }
        player->update(deltaTime, obj_map);
        boss->update(deltaTime, obj_map);
    }
    currCamera->Update();
}

void mainInit() {
    CreateShader();
    CreateShader_obj();

    //ºû
    directionalLight = new DirectionalLight(0.3f, 1.f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(0.0f, -1.0f, 0.f));
    entityList.push_back(directionalLight);
   /*pointLights[0] = new PointLight
    (0.f, 1.f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(1.f, 1.f, 0.0f),
        0.5f, 0.01f, 0.001f);
    pointLightCount++;*/
   /*pointLights[1] = new PointLight
    (0.0f, 0.5f,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec3(-2.0f, 2.0f, -1.f),
        1.0, 0.045f, 0.0075f);
    pointLightCount++;*/
   for (int i = 0; i < pointLightCount; i++)
        entityList.push_back(pointLights[i]);

    // Skybox
   {
       std::vector<std::string> skyboxFaces;
       skyboxFaces.push_back("Skybox/px.png");
       skyboxFaces.push_back("Skybox/nx.png");
       skyboxFaces.push_back("Skybox/py.png");
       skyboxFaces.push_back("Skybox/ny.png");
       skyboxFaces.push_back("Skybox/pz.png");
       skyboxFaces.push_back("Skybox/nz.png");
       skybox = new Skybox(skyboxFaces);
   }

    // Model loading
    mainModel = new Model();
    std::string modelPath = "Player/player.gltf";
    mainModel->LoadModel(modelPath);
    //¸ðµ¨ 90µµ È¸Àü
    GLfloat* currRot = mainModel->GetRotate();
    float rotation = 90;
    float newRotx = currRot[0] + rotation;
    glm::vec3 newRot(newRotx, currRot[1], currRot[2]);
    mainModel->SetRotate(newRot);
    entityList.push_back(mainModel);
    //ÇÃ·¹ÀÌ¾î ¿¬°á
    collide_box = new Model();
    std::string modelPath_collide = "collide_box/collide_box.gltf";
    collide_box->LoadModel(modelPath_collide);
    collide_box->SetRotate({ 0,0,0 });
    collide_box->SetScale(glm::vec3(0.4, 1.1, 0.4));
    player = new Player(mainModel, collide_box);

    //----------------------------------------
    std::random_device rd;  // ÇÏµå¿þ¾î ³­¼ö »ý¼º±â
    std::mt19937 gen(rd());  // Mersenne Twister ¿£Áø
    std::uniform_int_distribution<> dis(-100, 100);
    //Ç® »ý¼º
    {
        // ¿ÀºêÁ§Æ® »ý¼º
        cube = new Model();
        modelPath = "Gress/ground.gltf";
        cube->LoadModel(modelPath);

        glm::vec3 newRot2(90, 0, 0);
        cube->SetRotate(newRot2);
        glm::vec3 newTns2(0, 0.5, 0);
        cube->SetTranslate(newTns2);
        glm::vec3 newscale1(0.5, 1, 0.5);
        cube->SetScale(newscale1);

        for (int i = 0; i < 500; i++) {
            int rand_x = dis(gen);
            int rand_z = dis(gen);
            //collide_box->SetTranslate({ rand_x ,0.5,rand_z });
            object = new Object("gress", cube, 0, 0, rand_x, rand_z, 1);
            obj_map[std::make_pair(rand_x, rand_z)] = object;
        }
    }
<<<<<<< HEAD
    //³ª¹« »ý¼º
    {
        modelPath = "Tree/tree.gltf";
        cube->LoadModel(modelPath);
        glm::vec3 newRot3(90, 0, 0);
        cube->SetRotate(newRot3);
        glm::vec3 newTns3(0, 9, 0);
=======

    modelPath = "Tree/tree.gltf";
    cube->LoadModel(modelPath);
    glm::vec3 newRot3(90, 0, 0);
    cube->SetRotate(newRot3);
    glm::vec3 newTns3(0, 9, 0);
    cube->SetTranslate(newTns3);
    glm::vec3 newscale3(10, 1, 10);
    cube->SetScale(newscale3);

    collide_box->SetScale({ 0.5, 9, 0.5});
    
    for (int i = 0; i < 100; i++) {
        int rand_x = dis(gen);
        int rand_z = dis(gen);
        collide_box->SetTranslate({ rand_x + 0.5 ,9,rand_z + 0.5 });
        object = new Object("tree", cube, collide_box, 0, rand_x + 0.5, rand_z + 0.5, 1);
        obj_map[std::make_pair(rand_x, rand_z)] = object;
        glm::vec3 newTns3(rand_x, 9, rand_z);
>>>>>>> 8cb7fee (ë³´ìŠ¤ ì›€ì§ìž„ (ì¶”ì  ë°°íšŒ ëŒ€ì‰¬ êµ¬í˜„))
        cube->SetTranslate(newTns3);
        glm::vec3 newscale3(10, 1, 10);
        cube->SetScale(newscale3);

        collide_box->SetScale({ 0.5, 9, 0.5 });

        for (int i = 0; i < 100; i++) {
            int rand_x = dis(gen);
            int rand_z = dis(gen);
            collide_box->SetTranslate({ rand_x+0.5 ,9,rand_z+0.5 });
            object = new Object("tree", cube, collide_box, 0, rand_x+0.5, rand_z+0.5, 1);
            obj_map[std::make_pair(rand_x, rand_z)] = object;
            glm::vec3 newTns3(rand_x , 9, rand_z );
            cube->SetTranslate(newTns3);
            entityList.push_back(cube);
        }
    }
    machine = new Model();
    modelPath = "machine/machine2.gltf";
    machine->LoadModel(modelPath);
    glm::vec3 newscale_machine(1, 1, 1);
    machine->SetScale(newscale_machine);
    machine->SetRotate({0,0,0});
    machine->SetTranslate({0,1,0});

    int rand_x = 2;//dis(gen);
    int rand_z = 0; // dis(gen);
    //collide_box->SetTranslate({ rand_x ,0,rand_z });
    object = new Object("machine", machine, 0, 0, rand_x, rand_z, 0 );
    obj_map[std::make_pair(rand_x, rand_z)] = object;
    
    //¶¥ »ý¼º
    {
        ground = new Model();
        modelPath = "Ground3/gnd_v0.gltf";
        //modelPath = "Ground/ground.gltf";
        ground->LoadModel(modelPath);

        currRot = ground->GetRotate();
        rotation = 90;
        newRotx = currRot[0] + rotation;
        glm::vec3 newRot4(newRotx, currRot[1], currRot[2]);
        ground->SetRotate(newRot4);

        glm::vec3 newTns4(0, 0, 0);
        ground->SetTranslate(newTns4);
        glm::vec3 newscale(100, 1, 100);
        ground->SetScale(newscale);
    }
    //º¸½º »ý¼º
    {
        boss_model = new Model();
        modelPath = "Boss/Boss.gltf";
        boss_model->LoadModel(modelPath);
        boss_model->SetRotate({ 0,180,0 });
        boss_model->SetScale({ 2,2,2 });
        boss_model->SetTranslate({ 0,0,5 });

        collide_box->SetScale(glm::vec3(0.8, 1.65, 0.8));
        boss = new Boss(boss_model, collide_box, player, obj_map);
        entityList.push_back(boss_model);
    }

    Model * title_obj = new Model();
    modelPath = "Title/title_image.gltf";
    title_obj->LoadModel(modelPath);
    title_obj->SetRotate({ 90,0,0 });
    title_obj->SetScale({2.5,1,1.5});
    title = new Object("title", title_obj, 0, 0, 0, 3, 0);


    freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 0.f), 100.f, 0.3f);
    eventCamera = new FreeCamera(glm::vec3(0.f, 0.f, 0.f), 100.f, 0.3f);
    playerCamera = new PlayerCamera(player);
    currCamera = eventCamera;


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

    //--------------

    // Clear the window
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    //¹«¾ð°¡ ¹ÙÀÎµå
    if (mode == "Play_mode" || mode =="Pause_mode" || mode =="Master_mode") {
        glm::mat4 viewMat = currCamera->GetViewMatrix();
        glm::mat4 projMat = currCamera->GetProjectionMatrix(screenWidth, screenHeight);
        glm::vec3 camPos = currCamera->GetPosition();

        skybox->DrawSkybox(viewMat, projMat);
        //¶¥ ±×¸®±â
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

        //³ª¹« ±×¸®±â
        for (const auto& obj : obj_map) {
            // obj.first´Â std::pair<int, int> Å¸ÀÔ (Å°)
            // obj.second´Â Object Å¸ÀÔ (°ª)
            obj.second->draw(currCamera, directionalLight, pointLights, pointLightCount);
        }

        player->draw(currCamera, directionalLight, pointLights, pointLightCount);
        boss->draw(currCamera, directionalLight, pointLights, pointLightCount);

    }
    else if (mode == "Title_mode") {
        
        title->draw(currCamera, directionalLight, pointLights, pointLightCount);
    }

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

    // ÀüÃ¼ È­¸éÀ¸·Î ÀüÈ¯
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

//¾È¾¸
void TimerFunction(int value) {
    //glutPostRedisplay(); // È­¸é Àç Ãâ·Â
    glutTimerFunc(1, TimerFunction, 1);
}