#include <iostream>
#include <GL/glew.h>
#include "Model.h"
#include "Player.h"
#include "Boss.h"
#include "Object.h"
#include "BossBehavior.h"
#include "CollisionManger.h"

#include <map>
BossBehavior::BossBehavior(Model* bossmodel, Model* playermodel,
	Collision* boss_c, Collision* player_c, 
	std::map<std::pair<int, int>, Object*> map) : MOVE_SPEED(5.f)
{
	model_b = bossmodel;
	model_p = playermodel;
	box_b = boss_c;
	box_p = player_c;
	this->map = map;
	
}

void BossBehavior::Wander(float deltaTime) {
	//-- 추적으로 변경
	if (InRange(324))
	{
		key = 1;
		turning_time = 0;
		return;
	}
	//--- 범위 내에 오기 전까지 순찰
	GLfloat currRotY = model_b->GetRotate()[1];
	GLfloat* currPos = model_b->GetTranslate();
	float distance = MOVE_SPEED * deltaTime;
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
				std::cout << "삐빅 객체가 있습니다!!!!!!\n";
				
				if (Collide(it->second->GetCollision(), delta))
				{
					is_crash = true;
				}
			}
		}
	}
	if (is_crash)
	{
		std::cout << "충돌하였습니다\n";
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
	}
	newPos = { currPos[0] + delta.x, currPos[1], currPos[2] + delta.z };
	model_b->SetTranslate(newPos);
}
void BossBehavior::Chase(float deltaTime) {
	//---근접공격으로 변경
	if (InRange(25))
	{
		key = 2;
		turning_time = 0;
		return;
	}
	Turn_to_Player();
	GLfloat currRotY = model_b->GetRotate()[1];
	GLfloat* currPos = model_b->GetTranslate();
	float distance = 1.2f * MOVE_SPEED * deltaTime;
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
				std::cout << "삐빅 객체가 있습니다!!!!!!\n";
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
		std::cout << "충돌하였습니다\n";
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
	}
	newPos = { currPos[0] + delta.x, currPos[1], currPos[2] + delta.z };
	model_b->SetTranslate(newPos);
}
void BossBehavior::closeAttack() {
//영역에 대한 화면 표시 및 충돌검사 필요

}
void BossBehavior::Dash(float deltaTime) {
	std::cout << "Dash\m";
	//---근접공격으로 변경
	if (InRange(25))
	{
		key = 2;
		turning_time = 0;
		return;
	}
	Turn_to_Player();
	GLfloat currRotY = model_b->GetRotate()[1];
	GLfloat* currPos = model_b->GetTranslate();
	float distance = 1.2f * MOVE_SPEED * deltaTime;
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
				std::cout << "삐빅 객체가 있습니다!!!!!!\n";
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
		std::cout << "충돌하였습니다\n";
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
	}
	newPos = { currPos[0] + delta.x, currPos[1], currPos[2] + delta.z };
	model_b->SetTranslate(newPos);
}
void BossBehavior::Slam() {
	std::cout << "Slam\m";
	//영역에 대한 화면 표시 및 충돌검사 필요
}

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
		Slam();
		break;
	default:
		break;
	}
}
void BossBehavior::Check_Paturn() {
	switch (key)
	{
	case 0:	//-- Wander();
		if (turning_time >= 2.0f)
		{
			float* curRot = GetBossModel()->GetRotate();
			GetBossModel()->SetRotate({ curRot[0],curRot[1] + 108,curRot[2] });
<<<<<<< HEAD
			turning_time[key] = 0;
			//reset_time();
=======
			turning_time = 0;
>>>>>>> 8cb7fee (蹂댁뒪 ���吏곸엫 (異붿쟻 諛고쉶 ����돩 援ы쁽))
		}
		break;
	case 1:		//-- Chase();
		if (turning_time >= 3.5f)
		{
			if (skill_order == 0)
			{
				key = 3;
			}
			else if (skill_order == 1)
			{
				key = 4;
			}
			turning_time = 0;
		}
		break;
	case 2:		//-- closeAttack();
		if (turning_time >= 3.0f)
		{
			key = 1;
			turning_time = 0;
		}
		break;
	case 3:		//-- Dash();
		if (turning_time >= 1.5f)
		{
			key = 1;
			turning_time = 0;
		}
		break;
	case 4:		//-- Slam();
		if (turning_time >= 2.0f)
		{
			key = 1;
			turning_time = 0;
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
void BossBehavior::SetKey(int goal) {
	key = goal;
}
void BossBehavior::Update(float deltaTime) {
	UpdateHitbox();
	if (!InRange(324) && key != 0)
	{
		key = 0;
		turning_time = 0;
	}
	turning_time += deltaTime;
	Check_Paturn();
	Do(deltaTime);
}
void BossBehavior::reset_time() {

}
bool BossBehavior::Collide(Collision* box, glm::vec3 delta) {
	Collision* nextbox = GetCollsion();;
	nextbox->NextPosition(delta);
	//X축 검사
	bool xOverlap = box->maxX >= nextbox->minX &&
		box->minX <= nextbox->maxX;
	//Y축 검사
	bool yOverlap = box->maxY >= nextbox->minY &&
		box->minY <= nextbox->maxY;
	//Z축 검사
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