#include <iostream>
#include <GL/glew.h>
#include "Model.h"
#include "Player.h"
#include "Boss.h"
#include "Object.h"
#include "BossBehavior.h"
#include "CollisionManger.h"

#include <map>
BossBehavior::BossBehavior(Model* bossmodel, Model* playermodel, Model* SlamEffect, 
    Collision* boss_c, Collision* player_c, std::map<std::pair<int, int>, Object*> map)
    : WALK_SPEED(4.f), RUN_SPEED(6.f), DASH_SPEED(12.5), SLAM_SPEED(8.0f)
{
    model_b = bossmodel;
    model_p = playermodel;
    box_b = boss_c;
    box_p = player_c;
    this->map = map;
    this->SlamEffect = SlamEffect;

    Dmg_SLAM = false;
    isSLAM = false;
}

void BossBehavior::Wander(float deltaTime) {
    //-- �������� ����
    if (InRange(324))
    {
        key = 1;
        turning_time = 0;
        return;
    }
    //--- ���� ���� ���� ������ ����
    GLfloat currRotY = model_b->GetRotate()[1];
    GLfloat* currPos = model_b->GetTranslate();
    float distance = WALK_SPEED * deltaTime;
    float dx = distance * sinf(glm::radians(currRotY));
    float dz = distance * cosf(glm::radians(currRotY));
    glm::vec3 delta(dx, 0, dz);
    glm::vec3 newPos(currPos[0] + delta.x, currPos[1], currPos[2] + delta.z);

    bool is_crash{};
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            auto it = map.find({ int(newPos[0]) + i, int(newPos[2]) + j });
            if (it != map.end() && it->second->GetCollision())
            {
                if (Collide(it->second->GetCollision(), delta))
                {
                    is_crash = true;
                }
            }
        }
    }
    if (is_crash)
    {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
    }
    newPos = { currPos[0] + delta.x, currPos[1], currPos[2] + delta.z };
    model_b->SetTranslate(newPos);
}
void BossBehavior::Chase(float deltaTime) {
    //---������������ ����
    if (InRange(16))
    {
        Turn_to_Player();
        key = 2;
        turning_time = 0;
        return;
    }
    Turn_to_Player();
    GLfloat currRotY = model_b->GetRotate()[1];
    GLfloat* currPos = model_b->GetTranslate();
    float distance = RUN_SPEED * deltaTime;
    float dx = distance * sinf(glm::radians(currRotY));
    float dz = distance * cosf(glm::radians(currRotY));
    glm::vec3 delta(dx, 0, dz);
    glm::vec3 newPos(currPos[0] + delta.x, currPos[1], currPos[2] + delta.z);

    bool is_crash{};
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            auto it = map.find({ int(newPos[0]) + i, int(newPos[2]) + j });
            if (it != map.end() && it->second->GetCollision())
            {
                UpdateHitbox();
                if (Collide(it->second->GetCollision(), delta))
                {
                    is_crash = true;
                }
            }
        }
    }
    if (is_crash)
    {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
    }
    newPos = { currPos[0] + delta.x, currPos[1], currPos[2] + delta.z };
    model_b->SetTranslate(newPos);
}
void BossBehavior::closeAttack() {
    // ���� Ÿ�̹� �� ���� �˻�
    if (turning_time >= 1.1 && turning_time <= 1.25) {
        if (InRange(36)) { // �Ÿ� ����
            float* Bpos = model_b->GetTranslate();
            float* Ppos = model_p->GetTranslate();
            float dx = Bpos[0] - Ppos[0];
            float dz = Bpos[2] - Ppos[2];

            // �÷��̾� ����(���� ����) ���
            float atk_angle = glm::degrees(atan2(dx, dz)) + 180; // �÷��̾� ����
            float view_angle = model_b->GetRotate()[1];   // ������ ȸ�� ����

            // ���� ���̸� 0 ~ 360�� ������ ��ȯ
            float angle_gap = fmod(view_angle - atk_angle + 360.0f, 360.0f);
            if (angle_gap > 180.0f) angle_gap -= 360.0f; // -180 ~ 180���� ����ȭ
            if (angle_gap >= -60.0f && angle_gap <= 60.0f) {
                if (!Dmg_Close_atk)
                {
                    Damage = 15.f;
                    Dmg_Close_atk = true;
                }
            }
        }
    }
}
void BossBehavior::Dash(float deltaTime) {
    //---������������ ����
    if (InRange(9))
    {
        key = 2;
        turning_time = 0;
        return;
    }
    Turn_to_Player();
    GLfloat currRotY = model_b->GetRotate()[1];
    GLfloat* currPos = model_b->GetTranslate();
    float distance = DASH_SPEED * deltaTime;
    float dx = distance * sinf(glm::radians(currRotY));
    float dz = distance * cosf(glm::radians(currRotY));
    glm::vec3 delta(dx, 0, dz);
    glm::vec3 newPos(currPos[0] + delta.x, currPos[1], currPos[2] + delta.z);

    bool is_crash{};
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            auto it = map.find({ int(newPos[0]) + i, int(newPos[2]) + j });
            if (it != map.end() && it->second->GetCollision())
            {
                UpdateHitbox();
                if (Collide(it->second->GetCollision(), delta))
                {
                    is_crash = true;
                }
            }
        }
    }
    if (is_crash)
    {
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
    }
    newPos = { currPos[0] + delta.x, currPos[1], currPos[2] + delta.z };
    model_b->SetTranslate(newPos);
}
void BossBehavior::Slam(float deltaTime) {
    if (turning_time >= 0.25 && turning_time <= 1.7)
    {
        GLfloat currRotY = model_b->GetRotate()[1];
        GLfloat* currPos = model_b->GetTranslate();
        float distance = SLAM_SPEED * deltaTime;
        float dx = distance * sinf(glm::radians(currRotY));
        float dz = distance * cosf(glm::radians(currRotY));
        glm::vec3 delta(dx, 0, dz);
        glm::vec3 newPos(currPos[0] + delta.x, currPos[1], currPos[2] + delta.z);
        model_b->SetTranslate(newPos);
        
    }
        //������ ���� ȭ�� ǥ�� �� �浹�˻� �ʿ�
};

void BossBehavior::Do(float deltaTime) {
    switch (key)
    {
    case 0:
        Wander(deltaTime);
        break;
    case 1:
        Chase(deltaTime);
        break;
    case 2:
        closeAttack();
        break;
    case 3:
        Dash(deltaTime);
        break;
    case 4:
        Slam(deltaTime);
        break;
    default:
        break;
    }
}
void BossBehavior::Check_Paturn() {
    switch (key)
    {
    case 0:   //-- Wander();
        if (turning_time >= 2.5f)
        {
            float* curRot = GetBossModel()->GetRotate();
            GetBossModel()->SetRotate({ curRot[0],curRot[1] + 90,curRot[2] });
            turning_time = 0;
        }
        break;
    case 1:      //-- Chase();
        if (turning_time >= 3.5f)
        {
            if (skill_order == 0)
            {
                key = 3;
                skill_order++;
            }
            else if (skill_order == 1)
            {
                key = 4;
                skill_order = 0;
            }
            turning_time = 0;
        }
        break;
    case 2:      //-- closeAttack();
        if (turning_time >= 2.0f)
        {
            key = 1;
            turning_time = 0;
            Dmg_Close_atk = false;
        }
        break;
    case 3:      //-- Dash();
        if (turning_time >= 2.5f)
        {
            key = 1;
            turning_time = 0;
        }
        break;
    case 4:      //-- Slam();
        if (turning_time >= 1.60f && turning_time <= 1.7f) {
            isSLAM = true;
            float* Bpos = model_b->GetTranslate();
            GLfloat currRotY = model_b->GetRotate()[1];
            float distance = WALK_SPEED;
            float dx = distance * sinf(glm::radians(currRotY));
            float dz = distance * cosf(glm::radians(currRotY));
            glm::vec3 delta(dx, 0, dz);
            glm::vec3 newPos(Bpos[0] + delta.x, Bpos[1], Bpos[2] + delta.z);

            SlamEffect->SetScale({ 0.01f,1,0.01f });
            SlamEffect->SetTranslate({ newPos[0], 0.001f, newPos[2] });
        }
        if (turning_time >= 2.5f)
        {
            key = 1;
            turning_time = 0;
            GLfloat currRotY = model_b->GetRotate()[1];
            GLfloat* currPos = model_b->GetTranslate();
            float distance = WALK_SPEED;
            float dx = distance * sinf(glm::radians(currRotY));
            float dz = distance * cosf(glm::radians(currRotY));
            glm::vec3 delta(dx, 0, dz);
            glm::vec3 newPos(currPos[0] + delta.x, currPos[1], currPos[2] + delta.z);
            model_b->SetTranslate(newPos);
        }
        break;
    default:
        break;
    }
}
bool BossBehavior::InRange(int distance) {
    float* Pposition = model_p->GetTranslate();
    float* Bposition = model_b->GetTranslate();

    float dx = Pposition[0] - Bposition[0];
    float dz = Pposition[2] - Bposition[2];

    return (dx * dx + dz * dz) <= distance;
}
void BossBehavior::Turn_to_Player() {
    float* Pposition = model_p->GetTranslate();
    float* Bposition = model_b->GetTranslate();
    float dx = Pposition[0] - Bposition[0];
    float dz = Pposition[2] - Bposition[2];

    model_b->SetRotate({ 0,glm::degrees(atan2(dx,dz)) ,0 });
}
void BossBehavior::updateSlam() {
    float* SScale = SlamEffect->GetScale();
    if (SScale[0] < 25.0f)
    {
        float size{ 0.5f };
        SlamEffect->SetScale({ SScale[0] + size,SScale[1],SScale[2] + size });
    }
    else
    {
        SlamEffect->SetScale({1,1,1});
        isSLAM = false;
        Dmg_SLAM = false;
    }

}
void BossBehavior::Update(float deltaTime) {
    
    if (isSLAM)
    {
        updateSlam();
        GetSlam();
    }
    UpdateHitbox();
    if (!InRange(324) && key != 0)
    {
        Turn_to_Player();
        key = 0;
        turning_time = 0;
    }
    turning_time += deltaTime;
    Check_Paturn();
    Do(deltaTime);
}
void BossBehavior::GetSlam() {
    if (!Dmg_SLAM && InRange_Slam())
    {
        if (model_p->GetTranslate()[1] == 0.f)
        {
            Dmg_SLAM = true;
            Damage = 15.0f;
        }
    }
}
bool BossBehavior::InRange_Slam() {
    float* Spos = SlamEffect->GetTranslate();
    float* Ssize = SlamEffect->GetScale();
    float* Ppos = model_p->GetTranslate();
    float dx = Spos[0] - Ppos[0];
    float dz = Spos[2] - Ppos[2];
    
    return (dx * dx + dz * dz) >= Ssize[0] * Ssize[0] - 4 &&
        (dx * dx + dz * dz) <= Ssize[0] * Ssize[0] + 4;
}

bool BossBehavior::Collide(Collision* box, glm::vec3 delta) {
    Collision* nextbox = GetCollsion();;
    nextbox->NextPosition(delta);
    //X�� �˻�
    bool xOverlap = box->maxX >= nextbox->minX &&
        box->minX <= nextbox->maxX;
    //Y�� �˻�
    bool yOverlap = box->maxY >= nextbox->minY &&
        box->minY <= nextbox->maxY;
    //Z�� �˻�
    bool zOverlap = box->maxZ >= nextbox->minZ &&
        box->minZ <= nextbox->maxZ;

    return xOverlap && yOverlap && zOverlap;
}
void BossBehavior::UpdateHitbox()
{
    GLfloat* modelTrans = model_b->GetTranslate();
    box_b->GetModel()->SetTranslate({ modelTrans[0],modelTrans[1] + box_b->GetModel()->GetScale()[1],modelTrans[2] });
    box_b->Update();
}