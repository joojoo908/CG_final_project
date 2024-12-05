
#include <GL/glew.h>
#include <iostream>
#include "Model.h"
#include "CollisionManger.h"


Collision::Collision(Model* model)
{
    this->model = model;
    GLfloat* currsize = model->GetScale();
    GLfloat* curPos = model->GetTranslate();
    float RotY = model->GetRotate()[1];
    minX = -currsize[0] + curPos[0];
    maxX = currsize[0] + curPos[0];
    minY = -currsize[1] + curPos[1];
    maxY = currsize[1] + curPos[1];
    minZ = -currsize[2] + curPos[2];
    maxZ = currsize[2] + curPos[2];
    

}
void Collision::Update() {
    GLfloat* currsize = model->GetScale();
    GLfloat* curPos = model->GetTranslate();

    minX = -currsize[0]  + curPos[0];
    maxX = currsize[0]  + curPos[0];
    minY = -currsize[1] + curPos[1];
    maxY = currsize[1] + curPos[1];
    minZ = -currsize[2]  + curPos[2];
    maxZ = currsize[2]  + curPos[2];
}
void Collision::NextPosition(glm::vec3 delta) {
    minX += delta.x;
    maxX += delta.x;
    minY += delta.y;
    maxY += delta.y;
    minZ += delta.z;
    maxZ += delta.z;
}

Collision::~Collision()
{

}