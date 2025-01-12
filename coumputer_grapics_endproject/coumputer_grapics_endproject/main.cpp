#include <iostream>
#include <random>
#include <vector>
#include <map>
#include<string>
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
#include "Boss.h"


int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
int Center_width = screenWidth / 2;
int Center_height = screenHeight / 2;

std::string mode = "Title_mode";
std::string back_mode = "";

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
Model* atk_circle;
Model* currModel;

Player* player;
std::vector<Object*> objs;
std::vector<Boss*> bosses;
std::map<std::pair<int, int>, Object*> obj_map;
Object* object;
Object* ship;
Object* cubes[4];
Object* title;
Object* hp_bar;
Object* pause;
Object* gameEnd;
Object* gameClear;

DirectionalLight* directionalLight;
DirectionalLight* directionalLight2;
DirectionalLight* directionalLight3;
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

void TimerFunction(int value);

//키보드 눌림 함수
void processKeyboard(unsigned char key, int x, int y) {
    if (mode == "Master_mode") {
        currCamera->KeyControl(key, deltaTime);
    }
    else if (mode == "Play_mode") {
        player->HandleInput(key, 1, deltaTime);
    }

    if (key == 'p') {
        if (mode == "Pause_mode") {
            mode = "Play_mode";
        }
        else {
            if (mode == "Play_mode") { 
                pause->update_pannel(currCamera);
                mode = "Pause_mode"; 
            }
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
}
//키보드 떼어짐 함수
void processKeyboardUp(unsigned char key, int x, int y) {
    if (mode == "Play_mode") {
        player->HandleInput(key, 0, deltaTime);
    }
}
//스페셜 키 함수
void SpecialKeyboard(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_F1:
       
        break;
    case GLUT_KEY_F2:
       
        break;
    case GLUT_KEY_F12:

        break;
    }
}

void processMouse(int x, int y) {
    GLfloat XChange = x - Center_width;
    GLfloat YChange = Center_height - y;
    if (mode != "Play_mode" && mode != "Master_mode") {
        XChange = 0;
        YChange = 0;
    }
    currCamera->MouseControl(XChange, YChange);

    if (mode == "Play_mode" || mode == "Master_mode") {
        if (!player->IsRoll() && !player->iswork())
        {
            player->MouseContrl(XChange, YChange);
        }
        glutWarpPointer(Center_width, Center_height);
    }


}

void Mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (mode == "Title_mode") {
            mode = "Play_mode";
            currCamera = playerCamera;
        }
        else if (mode == "Pause_mode") {
            //마우스 피킹
            float px = (float)(x - Center_width) / (float)Center_width;
            float py = (float)(Center_height - y) / (float)Center_height;
            if (px > -0.28 && px < 0.28 && py>0.11 &&py<0.38) {
                std::cout << "다시 시작" << std::endl;
                mode = "Play_mode";
            }
            if (px > -0.28 && px<0.28 && py>-0.43 && py < -0.16) {
                std::cout << "종료" << std::endl;
                glutLeaveMainLoop();
            }
        }
        else if (mode == "End_mode" || mode =="Clear_mode") {
            glutLeaveMainLoop();
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
//---------------------------------------------------------------

void update() {

    if (mode == "Play_mode" || back_mode == "Play_mode") {
        for (const auto& obj : obj_map) {
            obj.second->update(deltaTime, currCamera->GetPosition());
        }
        for (auto boss : bosses) {
            boss->update(deltaTime, obj_map);
        }

        switch (player->GetKey())
        {
        case 0:
            pointLights[1]->color = glm::vec4(0.5, 0, 0, 1);
            player->SetKey();
            break;
        case 1:
            pointLights[2]->color = glm::vec4(0, 0.5, 0, 1);
            player->SetKey();
            break;
        case 2:
            pointLights[3]->color = glm::vec4(0, 0, 0.5, 1);
            player->SetKey();
            break;
        case 3:
            pointLights[4]->color = glm::vec4(0.5, 0, 0.5, 1);
            player->SetKey();
            break;
        case 4:
            pointLights[0]->color = glm::vec4(1.f, 1.f, 1.f, 1);
            player->SetKey();
            break;
        default:
            break;
        }
        player->update(deltaTime, obj_map);
        if (player->IS_DEAD()) // 게임오버 조건
        {
            mode = "End_mode";
            currCamera = eventCamera;
        }
        else if (player->IS_END()) //클리어 조건
        {
            mode = "Clear_mode";
            currCamera = eventCamera;
        }
        
    }
    currCamera->Update();
    hp_bar->update_hp(currCamera,player->GetHp());

}

void mainInit() {
    CreateShader();
    CreateShader_obj();

    //전체 빛
    {
        directionalLight = new DirectionalLight(0.5f, 1.f,
            glm::vec4(1.f, 1.f, 1.f, 1.f),
            glm::vec3(0.0f, 0.0f, 0.0f));

        directionalLight2 = new DirectionalLight(1.f, 1.f,
            glm::vec4(1.f, 1.f, 1.f, 1.f),
            glm::vec3(0.0f, 0.0f, 0.0f));

        directionalLight3 = new DirectionalLight(0.3f, 0.f,
            glm::vec4(1.f, 1.f, 1.f, 1.f),
            glm::vec3(0.0f, -1.0f, 0.0f));

    }
    //포인트 빛
    {
        pointLights[0] = new PointLight(
            0.f, 1.f,
            glm::vec4(0.f, 0.f, 0.f, 1.f),
            glm::vec3(0.f, 1.f, 0.f), // 바닥에 위치한 점광원
            0.2f,   // constant
            0.001f,   // linear
            0.001f   // exponent
        );
        pointLightCount++;

        pointLights[1] = new PointLight
        (0.0f, 0.5f,
            glm::vec4(0.f, 0.f, 0.f, 1.f),
            glm::vec3(60.0f, 1.0f, 60.f),
            0.2, 0.001f, 0.001f);
        pointLightCount++;

        pointLights[2] = new PointLight
        (0.0f, 0.5f,
            glm::vec4(0.f, 0.f, 0.f, 1.f),
            glm::vec3(-60.0f, 1.0f, 60.f),
            0.2, 0.001f, 0.001f);
        pointLightCount++;

        pointLights[3] = new PointLight
        (0.0f, 0.5f,
            glm::vec4(0.f, 0.f, 0.f, 1.f),
            glm::vec3(60.0f, 1.0f, -60.f),
            0.2, 0.001f, 0.001f);
        pointLightCount++;

        pointLights[4] = new PointLight
        (0.0f, 0.5f,
            glm::vec4(0.f, 0.f, 0.f, 1.f),
            glm::vec3(-60.0f, 1.0f, -60.f),
            0.2, 0.001f, 0.001f);
        pointLightCount++;
    }
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

    //모델 추가용 init() 함수 추가 예정
    
    
    std::random_device rd;  // 하드웨어 난수 생성기
    std::mt19937 gen(rd());  // Mersenne Twister 엔진
    std::uniform_int_distribution<> dis(-99, 99);



    std::string modelPath = "Wall/wall.gltf";
    std::string modelPath2 = "collide_box/collide_box.gltf";

    collide_box = new Model();
    collide_box->LoadModel(modelPath2);
    collide_box->SetRotate({ 0,0,0 });
    collide_box->SetScale(glm::vec3(1.0f, 1.0f, 1.0f));

    //벽
    {
        Model* wall = new Model();
        wall->LoadModel(modelPath);
        wall->SetTranslate({0,2,0});
        wall->SetScale({ 1,1,2 });
        wall->SetRotate({ 90,90,0 });
        collide_box->SetScale({ 0.2f, 2.f, 1.0f });
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 199; i+=2)
            {
                collide_box->SetTranslate({ -100 + 200 * j,2,-99 + i });
                object = new Object("wall", wall, collide_box, 0, -100 + j * 200, -99 + i, 0);
                obj_map[std::make_pair(-100 + j * 200, -99 + i)] = object;
                obj_map[std::make_pair(-100 + j * 200, -99 + i-1)] = object;
            }
        }

        wall->SetRotate({ 90,0,0 });
        collide_box->SetScale({ 1.f, 2.f, 0.2 });
        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 199; i+=2)
            {
                collide_box->SetTranslate({ -100 + i,2, -100 + 200 * j });
                object = new Object("wall", wall, collide_box, 0, -99 + i, -100 + 200 * j, 0);
                obj_map[std::make_pair(-99 + i, -100 + j * 200)] = object;
                obj_map[std::make_pair(-99 + i-1, -100 + j * 200)] = object;
            }
        }
    }
    //풀
    {
        cube = new Model();
        modelPath = "Gress/ground.gltf";
        cube->LoadModel(modelPath);

        glm::vec3 newRot2(90, 0, 0);
        cube->SetRotate(newRot2);
        glm::vec3 newTns2(0, 0.5, 0);
        cube->SetTranslate(newTns2);
        glm::vec3 newscale1(0.5, 1, 0.5);
        cube->SetScale(newscale1);

        for (int i = 0; i < 100; i++) {
            int rand_x = dis(gen);
            int rand_z = dis(gen);
            object = new Object("gress", cube, 0, 0, rand_x, rand_z, 1);
            obj_map[std::make_pair(rand_x, rand_z)] = object;
        }
    }
    //나무
    {
        modelPath = "Tree/tree.gltf";
        cube->LoadModel(modelPath);
        glm::vec3 newRot3(90, 0, 0);
        cube->SetRotate(newRot3);
        glm::vec3 newTns3(0, 9, 0);
        cube->SetTranslate(newTns3);
        glm::vec3 newscale3(10, 1, 10);
        cube->SetScale(newscale3);

        collide_box->SetScale({ 0.5, 9, 0.5 });

        for (int i = 0; i < 300; i++) {
            int rand_x = dis(gen);
            int rand_z = dis(gen);
            bool b0 = (rand_x > 60 - 5 && rand_x < 60 + 5 && rand_z > 60 - 5 && rand_z < 60 + 5);
            bool b1 = (rand_x > 60 - 5 && rand_x < 60 + 5 && rand_z > -60 - 5 && rand_z < -60 + 5);
            bool b2 = (rand_x > -60 - 5 && rand_x < -60 + 5 && rand_z > 60 - 5 && rand_z < 60 + 5);
            bool b3 = (rand_x > -60 - 5 && rand_x < -60 + 5 && rand_z > -60 - 5 && rand_z < -60 + 5);
                if ( !b0&&!b1&&!b2&&!b3 ) {
                    collide_box->SetTranslate({ rand_x + 0.5 ,9,rand_z + 0.5 });
                    object = new Object("tree", cube, collide_box, 0, rand_x + 0.5, rand_z + 0.5, 1);
                    obj_map[std::make_pair(rand_x, rand_z)] = object;
                    glm::vec3 newTns3(rand_x, 9, rand_z);
                    cube->SetTranslate(newTns3);
                    entityList.push_back(cube);
                }
        }
        //플레이어 소환 위치 나무 제거
        obj_map.erase(std::make_pair(0, 0));
        obj_map.erase(std::make_pair(0, -1));
        obj_map.erase(std::make_pair(-1, 0));
        obj_map.erase(std::make_pair(-1, -1));
    }
    //머신
    {
        machine = new Model();
        modelPath = "machine/machine2.gltf";
        machine->LoadModel(modelPath);
        glm::vec3 newscale_machine(1, 1, 1);
        machine->SetScale(newscale_machine);
        machine->SetRotate({ 0,0,0 });
        machine->SetTranslate({ 0,1,0 });

        collide_box->SetRotate({ 0,0,0 });
        collide_box->SetScale(glm::vec3(0.7, 0.6, 0.4));

        collide_box->SetTranslate({ 60.5,0.6,60.5 });
        object = new Object("machine", machine, collide_box, 0, 60.5, 60.5, 0);
        obj_map[std::make_pair(60, 60)] = object;

        collide_box->SetTranslate({ 60.5,0.6,-60.5 });
        object = new Object("machine", machine, collide_box, 0, 60.5,- 60.5, 0);
        obj_map[std::make_pair(60, -60)] = object;

        collide_box->SetTranslate({ -60.5,0.6,60.5 });
        object = new Object("machine", machine, collide_box, 0, -60.5, 60.5, 0);
        obj_map[std::make_pair(-60, 60)] = object;

        collide_box->SetTranslate({ -60.5,0.6,-60.5 });
        object = new Object("machine", machine, collide_box, 0, -60.5, -60.5, 0);
        obj_map[std::make_pair(-60, -60)] = object;
    }
    //땅
    {
        ground = new Model();
        modelPath = "Ground3/gnd_v0.gltf";
        ground->LoadModel(modelPath);
        ground->SetRotate({ 0,0,0 });
        glm::vec3 newscale(100, 1, 100);
        ground->SetScale(newscale);
    }
    //함선
    {
        Model* m = new Model();
        modelPath = "Ship/ship2.gltf";
        m->LoadModel(modelPath);
        m->SetScale({ 0.04,0.04,0.04 });
        m->SetRotate({ 0,90,0 });
        m->SetTranslate({ 0,50,0 });
        ship = new Object("ship", m, 0, 0, 0, 0, 0);
    }
    //Atk Circle
    {
        atk_circle = new Model();
        modelPath = "Atk_circle/atk_circle.gltf";
        atk_circle->LoadModel(modelPath);
        atk_circle->SetRotate({ 0,0,0 });
        atk_circle->SetScale({ 1,1,1 });
        atk_circle->SetTranslate({ 0,0.1f,0 });
    }
    //플레이어
    {
        mainModel = new Model();
        modelPath = "Player/player2.gltf";
        mainModel->LoadModel(modelPath);
        //모델 90도 회전
        GLfloat* currRot = mainModel->GetRotate();
        float rotation = 90;
        float newRotx = currRot[0] + rotation;
        glm::vec3 newRot(newRotx, currRot[1], currRot[2]);
        mainModel->SetRotate(newRot);
        entityList.push_back(mainModel);

        collide_box->SetTranslate({ 0,0,0 });
        collide_box->SetRotate({ 0,0,0 });
        collide_box->SetScale(glm::vec3(0.4, 1.1, 0.4));
        player = new Player(mainModel, collide_box, obj_map);
    }
    //체력바
    {
        Model* m = new Model();
        modelPath = "Hp/hpgltf.gltf";
        m->LoadModel(modelPath);
        m->SetScale({ 0.1,0.1,0.1 });
        m->SetRotate({ 0,90,0 });
        m->SetTranslate({ 0,50,0 });
        hp_bar = new Object("hp", m, 0, 0, 0, 0, 0);
    }
    //보스
    {
        boss_model = new Model();
        modelPath = "Boss/Boss.gltf";
        boss_model->LoadModel(modelPath);
        boss_model->SetRotate({ 0,180,0 });
        boss_model->SetScale({ 2,2,2 });

        collide_box->SetScale(glm::vec3(0.8, 1.65, 0.8));

        boss_model->SetTranslate({ 65,0,65 });
        Boss * boss = new Boss(boss_model, collide_box, atk_circle, player, obj_map);
        bosses.push_back(boss);

        boss_model->SetTranslate({ 65,0,-55 });
        Boss* boss2 = new Boss(boss_model, collide_box, atk_circle, player, obj_map);
        bosses.push_back(boss2);
        boss_model->SetTranslate({ -55,0,65 });
        Boss* boss3 = new Boss(boss_model, collide_box, atk_circle, player, obj_map);
        bosses.push_back(boss3);
        boss_model->SetTranslate({ -55,0,-55 });
        Boss* boss4 = new Boss(boss_model, collide_box, atk_circle, player, obj_map);
        bosses.push_back(boss4);

    }
    //타이틀 화면
    {
        Model* title_obj = new Model();
        modelPath = "Title/title_image.gltf";
        title_obj->LoadModel(modelPath);
        title_obj->SetRotate({ 90,0,0 });
        title_obj->SetScale({ 2.5,1,1.5 });
        title = new Object("title", title_obj, 0, 0, 0, 3, 0);
    }
    //정지 화면
    {
        Model* pause_obj = new Model();
        modelPath = "Pause/pause_image.gltf";
        pause_obj->LoadModel(modelPath);
        pause_obj->SetRotate({ 90,180,0 });
        //pause_obj->SetScale({ 1,1,1 });
        pause = new Object("pause", pause_obj, 0, 0, 0, 3, 0);
    }
    //종료 화면
    {
        Model* title_obj = new Model();
        modelPath = "Game_over/end_image.gltf";
        title_obj->LoadModel(modelPath);
        title_obj->SetRotate({ 90,180,0 });
        title_obj->SetScale({ 2.5,1,1.5 });
        gameEnd = new Object("gameEnd", title_obj, 0, 0, 0, 3, 0);
    }
    //종료 화면
    {
        Model* title_obj = new Model();
        modelPath = "Game_clear/clear_image.gltf";
        title_obj->LoadModel(modelPath);
        title_obj->SetRotate({ 90,180,0 });
        title_obj->SetScale({ 2.5,1,1.5 });
        gameClear = new Object("gameClear", title_obj, 0, 0, 0, 3, 0);
    }
    freeCamera = new FreeCamera(glm::vec3(0.f, 0.f, 0.f), 100.f, 0.3f);
    eventCamera = new FreeCamera(glm::vec3(0.f, 0.f, 0.f), 100.f, 0.3f);
    playerCamera = new PlayerCamera(player);
    currCamera = eventCamera;

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

    if (mode == "Play_mode" || mode =="Pause_mode" || mode =="Master_mode") {

        glm::mat4 viewMat = currCamera->GetViewMatrix();
        glm::mat4 projMat = currCamera->GetProjectionMatrix(screenWidth, screenHeight);
        glm::vec3 camPos = currCamera->GetPosition();

        skybox->DrawSkybox(viewMat, projMat);
    
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
            shaderList[1]->UseDirectionalLight(directionalLight3);
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

        //나무 그리기
        for (const auto& obj : obj_map) {
            // obj.first는 std::pair<int, int> 타입 (키)
            // obj.second는 Object 타입 (값)
            obj.second->draw(currCamera, directionalLight, pointLights, pointLightCount);
        }
        //보스들 그리기
        for (auto boss : bosses) {
            boss->draw(currCamera, directionalLight, pointLights, pointLightCount);
        }

        player->draw(currCamera, directionalLight, pointLights, pointLightCount);
       
        //배틀쉽 그리기
        ship->draw(currCamera, directionalLight, pointLights, pointLightCount);

        if (mode == "Pause_mode") {
            glDisable(GL_DEPTH_TEST);
            pause->draw(currCamera, directionalLight2, pointLights, pointLightCount);
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
            hp_bar->draw(currCamera, directionalLight2, pointLights, pointLightCount);
            glEnable(GL_DEPTH_TEST);
        }

    }
    else if (mode == "Title_mode") {
        
        title->draw(currCamera, directionalLight2, pointLights, pointLightCount);

    }
    else if (mode == "End_mode") {
        gameEnd->draw(currCamera, directionalLight2, pointLights, pointLightCount);
    }
    else if (mode == "Clear_mode") {
        gameClear->draw(currCamera, directionalLight2, pointLights, pointLightCount);
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


    // 전체 화면으로 전환
    glutFullScreen();

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