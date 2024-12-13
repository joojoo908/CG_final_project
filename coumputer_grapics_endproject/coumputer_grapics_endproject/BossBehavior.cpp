#include <iostream>
#include <GL/glew.h>
#include "Model.h"
#include "Player.h"
#include "Boss.h"
#include "BossBehavior.h"

#include <map>
BossBehavior::BossBehavior(Model* bossmodel, Model* playermodel, std::map<std::pair<int, int>, Object*> map) : MOVE_SPEED(5.f)
{
	model_b = bossmodel;
	model_p = playermodel;
	reset_time();
}

void BossBehavior::Wander(float deltaTime) {
	//-- 추적으로 변경
	if (InRange(324))
	{
		key = 1;
		reset_time();
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
	model_b->SetTranslate(newPos);
	


}
void BossBehavior::Chase(float deltaTime) {
	//---근접공격으로 변경
	if (InRange(25))
	{
		key = 2;
		reset_time();
		return;
	}
	Turn_to_Player();
	GLfloat currRotY = model_b->GetRotate()[1];
	GLfloat* currPos = model_b->GetTranslate();
	float distance = 1.3f * MOVE_SPEED * deltaTime;
	float dx = distance * sinf(glm::radians(currRotY));
	float dz = distance * cosf(glm::radians(currRotY));
	glm::vec3 delta(dx, 0, dz);
	glm::vec3 newPos(currPos[0] + delta.x, currPos[1], currPos[2] + delta.z);
	model_b->SetTranslate(newPos);
}
void BossBehavior::closeAttack() {
//영역에 대한 화면 표시 및 충돌검사 필요

}

void BossBehavior::Dash() {
	//영역에 대한 화면 표시 및 충돌검사 필요
}
void BossBehavior::Slam() {
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
		Dash();
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
		if (turning_time[key] >= 2.0f)
		{
			float* curRot = GetBossModel()->GetRotate();
			GetBossModel()->SetRotate({ curRot[0],curRot[1] + 108,curRot[2] });
			turning_time[key] = 0;
			//reset_time();
		}
		break;
	case 1:		//-- Chase();
		//if (turning_time[key] >= 1.0f)
		//{
		//	key = 2;
		//	reset_time();
		//}
		break;
	case 2:		//-- closeAttack();
		if (turning_time[key] >= 3.0f)
		{
			key = 1;
			reset_time();
		}
		break;
	case 3:		//-- Dash();
		if (turning_time[key] >= 1.0f)
		{
			reset_time();
		}
		break;
	case 4:		//-- Slam();
		if (turning_time[key] >= 1.0f)
		{

			reset_time();
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
	
	if (!InRange(324) && key != 0)
	{
		key = 0;
		reset_time();
	}
	turning_time[key] += deltaTime;
	Check_Paturn();
	Do(deltaTime);
}
void BossBehavior::reset_time() {
	for (int i = 0; i < 5; i++)
	{
		turning_time[i] = 0;
	}
}