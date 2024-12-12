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

Player::Player(Model* model,Model* hitbox) : MOVE_SPEED(10.f), TURN_SPEED(0.5f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	this->hitbox = new Model(*hitbox);
	this->collisionbox = new Collision(this->hitbox);
	this->animator = new Animator(nullptr);
	
	groundHeight = 10;
	upwardSpeed = 0;

	idleAnim = new Animation("Player/player.gltf", model);
	danceAnim = new Animation("Knight/dance.gltf", model);
	runAnim = new Animation("Player/player.gltf", model);

	isJumping = true;
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
	glm::vec3 newRot(currRot[0], newRotY, currRot[2]);
	model->SetRotate(newRot);
	//hitbox->SetRotate(newRot);



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

	// 새로운 위치 계산
	glm::vec3 delta(dx, upwardSpeed, dz);
	glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

	//맵 오브젝트들과 충돌검사 (콜리전박스 있는애들만)
	bool canmove = true;
	//auto it1 = map.find({ int(currPos[0]), int(currPos[2]) });
	auto it = map.find({ int(newPos[0]), int(newPos[2]) });
	if (it != map.end())
	{
		UpdateHitbox();
		if (Collide(it->second->GetCollision(), delta))
		{
			canmove = false;      
		}
	}
	/*for (const auto& obj : map)
	{
		if (InRange(obj.first,10) && obj.second->GetCollision())
		{
			std::cout << "x: " << obj.first.first <<", z: " << obj.first.second << "\n";
			UpdateHitbox();
			if (Collide(obj.second->GetCollision(), delta))
			{	
				canmove = false;
			}
		}
	}*/
	if (canmove)
	{
		model->SetTranslate(newPos);
		newPos.y += hitbox->GetScale()[1];
		hitbox->SetTranslate(newPos);
	}


	// 충돌 처리 (중력 및 땅과의 충돌 포함)
	// groundHeight = terrain->GetHeight(newPos.x, newPos.z);
	// if (newPos[1] <= groundHeight) {
	//     upwardSpeed = 0;
	//     newPos[1] = groundHeight;
	//     isJumping = false;
	// }

	// 새로운 위치 설정

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
//bool Player::InRange(int x1, int z1, int x2, int z2) {
//	
//
//	
//}



void Player::UpdateHitbox() 
{
	GLfloat* modelRot = model->GetRotate();
	GLfloat* modelTrans = model->GetTranslate();
	std::cout << "x: " << modelTrans[0] << ", y: " << modelTrans[1] << ", z: " << modelTrans[2] << "\n";
	
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

void Player::Jump()
{
	if (isJumping)
		return;

	upwardSpeed = JUMP_POWER;
	isJumping = true;
}

void Player::update(float deltaTime, std::map<std::pair<int, int>, Object*> map) {

	if (Move(deltaTime,map))
	{
		if (animator->GetCurrAnimation() != runAnim)
			animator->PlayAnimation(runAnim);
	}
	else
	{
		if (animator->GetCurrAnimation() != idleAnim)
			animator->PlayAnimation(idleAnim);
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