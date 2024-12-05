#include "Boss.h"

#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "CollisionManger.h"
#include "ShaderHandle.h"
#include "Object.h"
#include "Player.h"
#include <map>

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Boss::Boss(Model* model, Model* hitbox, Player* player) : MOVE_SPEED(5.f), TURN_SPEED(0.5f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	this->hitbox = new Model(*hitbox);
	this->collisionbox = new Collision(this->hitbox);
	this->animator = new Animator(nullptr);
	this->player = player;

	groundHeight = 10;
	upwardSpeed = 0;

	idleAnim = new Animation("Boss/Boss.gltf", model);
	runAnim = new Animation("Boss/boss_run.gltf", model);
	atkAnim = new Animation("Boss/boss_atk.gltf", model);

	isJumping = true;
}

bool Boss::Move(float deltaTime, std::map<std::pair<int, int>, Object*> map)
{
	// 현재 회전값과 위치 가져오기
	GLfloat* currRot = model->GetRotate();
	GLfloat* currPos = model->GetTranslate();

	float distance{};
	// 이동 거리 및 회전 계산
	if (InRange(144))
	{
		Turn_to_Player();
		distance = MOVE_SPEED * deltaTime;
	}

	//float dir_Rot = currRot[1] + glm::degrees(atan2f(currMoveSpeed_x, currMoveSpeed_z));

	// 라디안으로 변환한 회전을 사용하여 dx와 dz 계산
	float dx = distance * sinf(glm::radians(currRot[1]));
	float dz = distance * cosf(glm::radians(currRot[1]));

	// 새로운 위치 계산
	glm::vec3 delta(dx, 0, dz);


	// 충돌 처리 (중력 및 땅과의 충돌 포함)
	// groundHeight = terrain->GetHeight(newPos.x, newPos.z);
	// if (newPos[1] <= groundHeight) {
	//     upwardSpeed = 0;
	//     newPos[1] = groundHeight;
	//     isJumping = false;
	// }

	//맵 오브젝트들과 충돌검사 (콜리전박스 있는애들만)
	bool iscrash = false;
	for (const auto& obj : map)
	{
		if (InRange(obj.first, 10) && obj.second->GetCollision())
		{
			UpdateHitbox();
			if (Collide(obj.second->GetCollision(), delta))
			{
				iscrash = true;
			}
		}
	}
	if (iscrash)
	{
		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		delta = glm::vec3(rotationMatrix * glm::vec4(delta, 1.0f));
	}
	glm::vec3 newPos(currPos[0] + delta.x, currPos[1] + upwardSpeed, currPos[2] + delta.z);
	
	model->SetTranslate(newPos);
	newPos.y += hitbox->GetScale()[1];
	hitbox->SetTranslate(newPos);
	// 이동 상태 반환
	return (currMoveSpeed_z != 0 || currMoveSpeed_x != 0);
}

bool Boss::InRange(const std::pair<int, int>& a,int distance) {
	int x = a.first;
	int z = a.second;

	const auto& translate = model->GetTranslate();
	float dx = translate[0] - x;
	float dz = translate[2] - z;

	// 거리 비교 (제곱근 계산 생략)
	return (dx * dx + dz * dz) <= distance;
}
bool Boss::InRange(int distance) {
	float* Pposition = this->player->GetModel()->GetTranslate();
	float* Bposition = model->GetTranslate();

	float dx = Pposition[0] - Bposition[0];
	float dz = Pposition[2] - Bposition[2];

	return (dx * dx + dz * dz) <= distance;
}
void Boss::UpdateHitbox()
{
	GLfloat* modelRot = model->GetRotate();
	GLfloat* modelTrans = model->GetTranslate();
	hitbox->SetTranslate({ modelTrans[0],modelTrans[1] + hitbox->GetScale()[1],modelTrans[2] });
	hitbox->SetRotate({ modelRot[0],modelRot[1] ,modelRot[2] });

	collisionbox->Update();
}

bool Boss::Collide(Collision* box, glm::vec3 delta) {

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

void Boss::Turn_to_Player() {
	float* Pposition = this->player->GetModel()->GetTranslate();
	float* Bposition = model->GetTranslate();

	float dx = Pposition[0] - Bposition[0];
	float dz = Pposition[2] - Bposition[2];

	model->SetRotate({ 0,glm::degrees(atan2(dx,dz)) ,0});

}


float Boss::GetRotY()
{
	return model->GetRotate()[1];
}

void Boss::Jump()
{
	if (isJumping)
		return;

	upwardSpeed = JUMP_POWER;
	isJumping = true;
}

void Boss::update(float deltaTime, std::map<std::pair<int, int>, Object*> map) {

	if (Move(deltaTime,map))
	{
		if (animator->GetCurrAnimation() != runAnim)
			animator->PlayAnimation(runAnim);
	}
	else
	{
		if (animator->GetCurrAnimation() != atkAnim)
			animator->PlayAnimation(atkAnim);
	}
	//애니메이션 업데이트
	animator->UpdateAnimation(deltaTime);

}

void Boss::draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount) {
	glm::mat4 viewMat = currCamera->GetViewMatrix();
	glm::mat4 projMat = currCamera->GetProjectionMatrix(1280, 720);
	glm::vec3 camPos = currCamera->GetPosition();

	shaderList[0]->UseShader();

	GetShaderHandles();

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

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}