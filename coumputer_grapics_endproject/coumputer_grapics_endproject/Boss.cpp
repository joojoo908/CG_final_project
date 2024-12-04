#include "Boss.h"

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

Boss::Boss(Model* model) : MOVE_SPEED(10.f), TURN_SPEED(0.5f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = model;

	this->animator = new Animator(nullptr);

	groundHeight = 10;
	upwardSpeed = 0;

	idleAnim = new Animation("Boss/Boss.gltf", model);
	danceAnim = new Animation("Knight/dance.gltf", model);
	runAnim = new Animation("Knight/run.gltf", model);

	isJumping = true;
}

bool Boss::Move(float deltaTime)
{
	// ���� ȸ������ ��ġ ��������
	GLfloat* currRot = model->GetRotate();
	GLfloat* currPos = model->GetTranslate();

	float distance{};
	// �̵� �Ÿ� �� ȸ�� ���
	if (currMoveSpeed_x != 0.f || currMoveSpeed_z != 0.f)
	{
		distance = MOVE_SPEED * deltaTime;
	}

	float dir_Rot = currRot[1] + glm::degrees(atan2f(currMoveSpeed_x, currMoveSpeed_z));

	// �������� ��ȯ�� ȸ���� ����Ͽ� dx�� dz ���
	float dx = distance * sinf(glm::radians(dir_Rot));
	float dz = distance * cosf(glm::radians(dir_Rot));

	// ���ο� ��ġ ���
	glm::vec3 newPos(currPos[0] + dx, currPos[1] + upwardSpeed, currPos[2] + dz);

	// �浹 ó�� (�߷� �� ������ �浹 ����)
	// groundHeight = terrain->GetHeight(newPos.x, newPos.z);
	// if (newPos[1] <= groundHeight) {
	//     upwardSpeed = 0;
	//     newPos[1] = groundHeight;
	//     isJumping = false;
	// }

	// ���ο� ��ġ ����
	model->SetTranslate(newPos);

	// �̵� ���� ��ȯ
	return (currMoveSpeed_z != 0 || currMoveSpeed_x != 0);
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

void Boss::update(float deltaTime) {

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
	//�ִϸ��̼� ������Ʈ
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
	//�ؽ�ó �ߺ� ���� �ذ�
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}