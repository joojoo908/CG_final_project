
#include <iostream>
#include <GL/glew.h>
//#include <GLFW/glfw3.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "Object.h"
#include "Player.h"
#include "Boss.h"
#include "BossBehavior.h"
#include "CollisionManger.h"

#include "ShaderHandle.h"

#include <map>

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Boss::Boss(Model* model, Model* hitbox, Model* SlamEffect, Player* player, std::map<std::pair<int, int>, Object*> map) : MOVE_SPEED(5.f), TURN_SPEED(0.5f), GRAVITY(0.2f), JUMP_POWER(0.05f)
{
	this->model = new Model(*model);
	this->hitbox = new Model(*hitbox);
	this->collisionbox = new Collision(this->hitbox);
	this->animator = new Animator(nullptr);
	this->SlamEffect = SlamEffect;
	this->player = player;
	this->behavior = new BossBehavior(this->model, player->GetModel(), SlamEffect,
	this->collisionbox,player->GetCollsion(), map);
	this->map = map; 
	groundHeight = 10;
	upwardSpeed = 0;

	idleAnim = new Animation("Boss/Boss.gltf", model);
	walkAnim = new Animation("Boss/boss_walk.gltf", model);
	runAnim = new Animation("Boss/boss_run.gltf", model);
	fastrunAnim = new Animation("Boss/boss_fastrun.gltf", model);
	atkAnim = new Animation("Boss/boss_atk.gltf", model);
	jumpAtkAnim = new Animation("Boss/boss_jumpAtk.gltf", model);
	deadAnim = new Animation("Boss/boss_dead.gltf", model);

	isJumping = true;
	inRange = false;
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

void Boss::update(float deltaTime, std::map<std::pair<int, int>, Object*> map)  {
	behavior->Update(deltaTime);
	float Damage = behavior->GetDamage();
	if (Damage > 0.f)
	{
		std::cout << Damage << std::endl;
		player->GetDamage(Damage);
		behavior->SetDamage(0.0f);
	}
	int key = behavior->GetKey();
	switch (key)
	{
	case 0:
		if (animator->GetCurrAnimation() != walkAnim) {
			animator->PlayAnimation(walkAnim);
		}
		break;
	case 1:
		if (animator->GetCurrAnimation() != runAnim) {
			animator->PlayAnimation(runAnim);
		}
		break;
	case 2:
		if (animator->GetCurrAnimation() != atkAnim)
			animator->PlayAnimation(atkAnim);
		break;
	case 3:
		if (animator->GetCurrAnimation() != fastrunAnim) {
			animator->PlayAnimation(fastrunAnim);
		}
		break;
	case 4:
		if (animator->GetCurrAnimation() != jumpAtkAnim)
			animator->PlayAnimation(jumpAtkAnim);
		break;
	default:
		break;
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
	
	// 슬램 이펙트그리기
	if (behavior->is_SLAM())
	{
		shaderList[1]->UseShader();
		GetShaderHandles_obj();

		glm::mat4 sMat = SlamEffect->GetModelMat();
		glm::mat4 sPVM = projMat * viewMat * sMat;
		glm::mat3 snormalMat = GetNormalMat(sMat);

		glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(sMat));
		glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(sPVM));
		glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(snormalMat));

		shaderList[1]->UseEyePos(camPos);
		shaderList[1]->UseDirectionalLight(directionalLight);
		shaderList[1]->UsePointLights(pointLights, pointLightCount);

		shaderList[1]->UseMaterial(SlamEffect->GetMaterial());
		SlamEffect->RenderModel();
	}
	

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}