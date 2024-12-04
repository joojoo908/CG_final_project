#include "Player.h"

#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "ShaderHandle.h"

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Player::Player(Model* model) : MOVE_SPEED(10.f), TURN_SPEED(0.5f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;
	
	this->animator = new Animator(nullptr);
	
	groundHeight = 10;
	upwardSpeed = 0;

	idleAnim = new Animation("Knight/idle.gltf", model);
	danceAnim = new Animation("Knight/dance.gltf", model);
	runAnim = new Animation("Knight/run.gltf", model);

	isJumping = true;
}

void Player::HandleInput(unsigned char keys, bool updown, float deltaTime)
{
	if (updown) {
		if (keys == 'w')
			currMoveSpeed_z = MOVE_SPEED;
		else if (keys == 's')
			currMoveSpeed_z = -MOVE_SPEED;
		else if (keys == 'a')
			currMoveSpeed_x = MOVE_SPEED;
		else if (keys == 'd')
			currMoveSpeed_x = -MOVE_SPEED;

		else
			currMoveSpeed_z = 0;

		if (keys == 'a') {

		}
		else if (keys == 'd') {

		}
	}
	else {
		if (keys == 'w')
			currMoveSpeed_z = 0;
		else if (keys == 's')
			currMoveSpeed_z = 0;
		if (keys == 'a')
			currMoveSpeed_x = 0;
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
}

bool Player::Move(float deltaTime)
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
	glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

	// 충돌 처리 (중력 및 땅과의 충돌 포함)
	// groundHeight = terrain->GetHeight(newPos.x, newPos.z);
	// if (newPos[1] <= groundHeight) {
	//     upwardSpeed = 0;
	//     newPos[1] = groundHeight;
	//     isJumping = false;
	// }

	// 새로운 위치 설정
	model->SetTranslate(newPos);

	// 이동 상태 반환
	return (currMoveSpeed_z != 0 || currMoveSpeed_x != 0);
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

void Player::update(float deltaTime) {

	if (Move(deltaTime))
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

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}