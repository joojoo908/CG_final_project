#include "Player.h"

#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "Object.h"
#include "ShaderHandle.h"
#include "CollisionManger.h"
#include <map>

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Player::Player(Model* model,Model* hitbox, std::map<std::pair<int, int>, Object*> map) : MOVE_SPEED(7.f), TURN_SPEED(0.5f), GRAVITY(0.8f), JUMP_POWER(0.4f)
{
	this->model = model;
	this->hitbox = new Model(*hitbox);
	this->collisionbox = new Collision(this->hitbox);
	this->animator = new Animator(nullptr);
	this->map = map;

	idleAnim = new Animation("Player/idle.gltf", model);
	runAnim = new Animation("Player/player.gltf", model);
	backRunAnim = new Animation("Player/backRun.gltf", model);
	leftRunAnim = new Animation("Player/leftRun.gltf", model);
	rightRunAnim = new Animation("Player/rightRun.gltf", model);
	jumpAnim = new Animation("Player/jump.gltf", model);
	deathAnim = new Animation("Player/death.gltf", model);
	sitAnim = new Animation("Player/sit.gltf", model);
	danceAnim = new Animation("Player/dance.gltf", model);
	
	groundHeight = 10;
	upwardSpeed = 0;
	working_time = {};
	ending_time = {};
	LightKey = 5;
	HP = 100.0f;

	isJumping = true;
	is_Live = true;
	can_escape = false;
	is_End = false;
	dance_time = 0;
}

void Player::HandleInput(unsigned char keys, bool updown, float deltaTime)
{
	if (updown) {
		if (keys == 'w')
			currMoveSpeed_z = MOVE_SPEED;
		else if (keys == 's')
			currMoveSpeed_z = -MOVE_SPEED;
		
		if (keys == 'a') {
			currMoveSpeed_x = MOVE_SPEED;
		}
		else if (keys == 'd')
			currMoveSpeed_x = -MOVE_SPEED;

		if (keys == 32) {
			Jump();
		}

		if (keys == 'f')
		{
			GLfloat* currPos = model->GetTranslate();
			GLfloat YRot = model->GetRotate()[1];
			std::cout << currPos[0] << ", " << currPos[2] << std::endl;
			int forwardX = int(currPos[0] + sinf(glm::radians(YRot))); // 전방 X 방향 (cosine 사용)
			int forwardZ = int(currPos[2] + cosf(glm::radians(YRot))); // 전방 Z 방향 (sine 사용)
			if (YRot >= -40 && YRot <= 40.f)
			{
				if (isMachine(forwardX, forwardZ))
				{
					is_Working = true;
				}
			}
		}
	}
	else {
		if (keys == 'w')
			currMoveSpeed_z = 0;
		else if (keys == 's')
			currMoveSpeed_z = 0;

		if (keys == 'a') {
			currMoveSpeed_x = 0;
		}
		else if (keys == 'd')
			currMoveSpeed_x = 0;
	}
	//점프 막음
	/*if (keys=='j')
		Jump();*/
}
void Player::MouseContrl(float XChange, float YChange) {
	// 회전
	GLfloat* currRot = model->GetRotate();
	float rotation = -TURN_SPEED * XChange;
	float newRotY = currRot[1] + rotation; // new rotY
	if (newRotY > 180)
	{
		newRotY -= 360.f;
	}
	else if (newRotY < -180){
		newRotY += 360.f;
	}

	glm::vec3 newRot(currRot[0], newRotY, currRot[2]);
	model->SetRotate(newRot);
	//hitbox->SetRotate(newRot);
}


bool Player::isMachine(int x, int z) {

	auto it = map.find({ x, z });
	if (it != map.end() && it->second->GetCollision())
	{
		UpdateHitbox();
		if (it->second->GetType() == "machine") {
			if (it->first.first > 0 && it->first.second > 0)
			{
				if (!is_Clear[0])
				{
					LightKey = 0;
					is_Clear[0] = true;
				}
				else
				{
					return false;
				}
			}
			else if (it->first.first < 0 && it->first.second > 0)
			{
				if (!is_Clear[1])
				{
					LightKey = 1;
					is_Clear[1] = true;
				}
				else
				{
					return false;
				}
			}
			else if (it->first.first < 0 && it->first.second < 0)
			{
				if (!is_Clear[2])
				{
					LightKey = 3;
					is_Clear[2] = true;
				}
				else
				{
					return false;
				}
			}
			else if (it->first.first > 0 && it->first.second < 0)
			{
				if (!is_Clear[3])
				{
					LightKey = 2;
					is_Clear[3] = true;
				}
				else
				{
					return false;
				}
			}
			return true;
		}
	}
	return false;
}

bool Player::Move(float deltaTime, std::map<std::pair<int, int>, Object*> map)
{
	// 현재 회전값과 위치 가져오기
	GLfloat* currRot = model->GetRotate();
	GLfloat* currPos = model->GetTranslate();
	
	float distance{};
	// 이동 거리 및 회전 계산
	if (currMoveSpeed_x != 0.f || currMoveSpeed_z != 0.f)
	{
		distance = MOVE_SPEED * deltaTime;
	}

	float dir_Rot = currRot[1] + glm::degrees(atan2f(currMoveSpeed_x, currMoveSpeed_z));
	
	// 라디안으로 변환한 회전을 사용하여 dx와 dz 계산
	float dx = distance * sinf(glm::radians(dir_Rot));
	float dz = distance * cosf(glm::radians(dir_Rot));


	upwardSpeed -= GRAVITY * deltaTime;
	// 새로운 위치 계산
	glm::vec3 delta(dx, upwardSpeed, dz);
	glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

	if (newPos[1] <= 0) // 땅에 닿았다면
	{
		upwardSpeed = 0;
		newPos[1] = 0;
		isJumping = false;
	}

	//맵 오브젝트들과 충돌검사 (콜리전박스 있는애들만)
	bool canmove = true;
	auto it1 = map.find({ int(currPos[0]), int(currPos[2]) });
	auto it = map.find({ int(newPos[0]), int(newPos[2]) });
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
					canmove = false;
				}
			}
		}
	}
	if (canmove)
	{
		model->SetTranslate(newPos);
		newPos.y += hitbox->GetScale()[1];
		hitbox->SetTranslate(newPos);
	}
	else {
		model->SetTranslate({ currPos[0] , newPos[1],currPos[2] });
		newPos.y += hitbox->GetScale()[1];
		hitbox->SetTranslate({ currPos[0] , newPos[1],currPos[2] });
	}

	// 이동 상태 반환
	return (currMoveSpeed_z != 0 || currMoveSpeed_x != 0);
}
bool Player::InRange(const std::pair<int, int>& a, int distance) {
	int x = a.first;
	int z = a.second;

	const auto& translate = model->GetTranslate();
	float dx = translate[0] - x;
	float dz = translate[2] - z;

	// 거리 비교 (제곱근 계산 생략)
	return (dx * dx + dz * dz) <= distance;
}

void Player::UpdateHitbox() 
{
	GLfloat* modelRot = model->GetRotate();
	GLfloat* modelTrans = model->GetTranslate();
	//std::cout << "x: " << modelTrans[0] << ", y: " << modelTrans[1] << ", z: " << modelTrans[2] << "\n";
	
	hitbox->SetTranslate({ modelTrans[0],modelTrans[1]+hitbox->GetScale()[1],modelTrans[2]});
	hitbox->SetRotate({ modelRot[0],modelRot[1] ,modelRot[2] });

	collisionbox->Update();
}

bool Player::Collide(Collision* box, glm::vec3 delta) {
	
	Collision* nextbox = GetCollsion();
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
float Player::GetRotY()
{
	return model->GetRotate()[1];
}
void  Player::GetDamage(float Damage) {
	HP -= Damage;
	if (HP <= 0.f)
	{
		is_Live = false;
	}
}
void Player::Jump()
{
	if (isJumping)
		return;

	upwardSpeed = JUMP_POWER;
	isJumping = true;
}

void Player::update(float deltaTime, std::map<std::pair<int, int>, Object*> map) {
	if (is_Working)
	{
		working_time += deltaTime;
		if (working_time >= 1.5f)
		{
			working_time = 0;
			is_Working = false;
		}
	}
	if (is_Live)
	{
		//오브젝트 상호작용
		if (is_Working)
		{
			dance_time = 0;
			if (animator->GetCurrAnimation() != sitAnim)
				animator->PlayAnimation(sitAnim);
		}
		else if (Move(deltaTime, map))
		{
			dance_time = 0;
			if (currMoveSpeed_z) {
				if (currMoveSpeed_z > 0) {
					if (animator->GetCurrAnimation() != runAnim)
						animator->PlayAnimation(runAnim);
				}
				else {
					if (animator->GetCurrAnimation() != backRunAnim)
						animator->PlayAnimation(backRunAnim);
				}
			}
			else {
				if (currMoveSpeed_x > 0) {
					if (animator->GetCurrAnimation() != leftRunAnim)
						animator->PlayAnimation(leftRunAnim);
				}
				else {
					if (animator->GetCurrAnimation() != rightRunAnim)
						animator->PlayAnimation(rightRunAnim);
				}
			}
		}
		else
		{
			if (isJumping) {
				if (animator->GetCurrAnimation() != jumpAnim)
					animator->PlayAnimation(jumpAnim);
			}
			else {
				
				if (dance_time>10) {
					if (animator->GetCurrAnimation() != danceAnim)
						animator->PlayAnimation(danceAnim);
				}
				else {
					if (animator->GetCurrAnimation() != idleAnim)
						animator->PlayAnimation(idleAnim);
				}

				dance_time += deltaTime;
			}
		}
	}
	//죽은 상태의 애니메이션
	else
	{
		ending_time += deltaTime;
		if (animator->GetCurrAnimation() != idleAnim)
			animator->PlayAnimation(idleAnim);
	}
	if (!can_escape)
	{
		OpenEnding();
	}
	else
	{
		ending_time += deltaTime;
		Ending();
	}
	//애니메이션 업데이트
	animator->UpdateAnimation(deltaTime);

}

void Player::draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount) {
	glm::mat4 viewMat = currCamera->GetViewMatrix();
	glm::mat4 projMat = currCamera->GetProjectionMatrix(1280, 720);
	glm::vec3 camPos = currCamera->GetPosition();

	shaderList[0]->UseShader();

	GetShaderHandles();

	if (currMoveSpeed_z != 0 && currMoveSpeed_x != 0) {
		GLfloat* currRot = model->GetRotate();
		float newRotY = currRot[1] + (currMoveSpeed_x / MOVE_SPEED)*(currMoveSpeed_z / MOVE_SPEED) * 45; // new rotY
		glm::vec3 newRot(currRot[0], newRotY, currRot[2]);
		model->SetRotate(newRot);
	}

	glm::mat4 modelMat = model->GetModelMat();
	glm::mat4 PVM = projMat * viewMat * modelMat;
	glm::mat3 normalMat = GetNormalMat(modelMat);

	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
	glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
	glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

	shaderList[0]->UseEyePos(camPos);
	shaderList[0]->UseDirectionalLight(directionalLight);
	shaderList[0]->UsePointLights(pointLights, pointLightCount);

	shaderList[0]->UseMaterial(model->GetMaterial());

	const auto& transforms = animator->GetFinalBoneMatrices();
	shaderList[0]->UseFinalBoneMatrices(transforms);

	glUniform1i(loc_diffuseSampler, 0);
	glUniform1i(loc_normalSampler, 1);


	
	model->RenderModel();
	//텍스처 중복 문제 해결
	glBindTexture(GL_TEXTURE_2D, 0);

	if (currMoveSpeed_z != 0 && currMoveSpeed_x != 0) {
		GLfloat* currRot = model->GetRotate();
		float newRotY = currRot[1] - (currMoveSpeed_x / MOVE_SPEED)*(currMoveSpeed_z / MOVE_SPEED) * 45; // new rotY
		glm::vec3 newRot(currRot[0], newRotY, currRot[2]);
		model->SetRotate(newRot);
	}

	
	
	//히트박스 그리기
	shaderList[1]->UseShader();

	GetShaderHandles_obj();

	glm::mat4 hitMat = hitbox->GetModelMat();
	glm::mat4 hitPVM = projMat * viewMat * hitMat;
	glm::mat3 hitnormalMat = GetNormalMat(hitMat);

	glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(hitMat));
	glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(hitPVM));
	glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(hitnormalMat));

	shaderList[1]->UseEyePos(camPos);
	shaderList[1]->UseDirectionalLight(directionalLight);
	shaderList[1]->UsePointLights(pointLights, pointLightCount);

	shaderList[1]->UseMaterial(hitbox->GetMaterial());


	hitbox->RenderModel();
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}
void Player::OpenEnding() {
	int cnt{};
	for (int i = 0; i < 4; i++)
	{
		if (is_Clear[i])
		{
			cnt++;
		}
	}
	if (cnt == 4)
	{
		LightKey = 4;
		can_escape = true;
	}
}
void Player::Ending() {
	float xPos = model->GetTranslate()[0];
	float zPos = model->GetTranslate()[2];
	
	if (InRange(xPos, zPos, 100))
	{
		is_End = true;
	}
}
bool Player::InRange(int x, int z,int goal) {
	return x * x + z * z <= goal;
}