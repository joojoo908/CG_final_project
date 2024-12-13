#include "Object.h"

#include <iostream>
#include <GL/glew.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "ShaderHandle.h"
#include "CollisionManger.h"

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Object::Object(std::string type,Model *model,Model *hitbox=NULL , Animator* animator = NULL , float x=0, float z=0 ,bool bilboard=0) : GRAVITY(0.2f)
{
	this->type = type;
	this->bilboard = bilboard;

	this->model = new Model(*model);
	if (hitbox)
	{
		this->hitbox = new Model(*hitbox);
		this->collisionbox = new Collision (this->hitbox);
	}
	
	if (!animator) {
		//std::cout << "null!!!" << std::endl;
	}
	else {
		this->animator = animator;
	}

	GLfloat* currPos = model->GetTranslate();
	glm::vec3 newTns3(x, currPos[1], z);
	this->model->SetTranslate(newTns3);
	
	
	/*GLfloat* currRot = this->model->GetRotate();
	float rotation = 180;
	float newRotx = currRot[0] + rotation;
	glm::vec3 newRot2(newRotx, currRot[1], currRot[2]);
	this->model->SetRotate(newRot2);*/
	
}

float Object::GetRotY()
{
	return model->GetRotate()[1];
}

void Object::update(float deltaTime, glm::vec3 v) {
	if (animator) {
		animator->UpdateAnimation(deltaTime);
	}

	if (bilboard) {
		GLfloat* currPos = model->GetTranslate();
		GLfloat angle = glm::atan(v.z - currPos[2], currPos[0] - v.x);
		model->SetRotate({ model->GetRotate()[0] ,  glm::degrees(angle) + 90 , model->GetRotate()[2] });
		GLfloat* currRot = model->GetRotate();
	}
}

void Object::update_pannel(CameraBase* currCamera) {

	glm::vec3 set = currCamera->GetFront();
	set.x *= 3;
	set.y *= 3;
	set.z *= 3;
	model->SetTranslate(currCamera->GetPosition()+set);

	GLfloat* currPos = model->GetTranslate();
	glm::vec3 modelPos = glm::vec3(currPos[0], currPos[1], currPos[2]);
	glm::vec3 v = currCamera->GetPosition();
	GLfloat angle = glm::atan(v.z - currPos[2], currPos[0] - v.x);

	glm::vec3 direction = glm::normalize(v - modelPos);
	GLfloat pitchAngle = glm::asin( direction.y );
	model->SetRotate({ 90 - glm::degrees(pitchAngle)  ,  glm::degrees(angle) - 90 , model->GetRotate()[2] });

}

void Object::draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount) {
	glm::mat4 viewMat = currCamera->GetViewMatrix();
	glm::mat4 projMat = currCamera->GetProjectionMatrix(1280, 720);
	glm::vec3 camPos = currCamera->GetPosition();

	if (animator) {
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
	}
	else{
		shaderList[1]->UseShader();

		GetShaderHandles_obj();

		glm::mat4 modelMat = model->GetModelMat();
		glm::mat4 PVM = projMat * viewMat * modelMat;
		glm::mat3 normalMat = GetNormalMat(modelMat);

		glUniformMatrix4fv(loc_modelMat, 1, GL_FALSE, glm::value_ptr(modelMat));
		glUniformMatrix4fv(loc_PVM, 1, GL_FALSE, glm::value_ptr(PVM));
		glUniformMatrix3fv(loc_normalMat, 1, GL_FALSE, glm::value_ptr(normalMat));

		shaderList[1]->UseEyePos(camPos);
		shaderList[1]->UseDirectionalLight(directionalLight);
		shaderList[1]->UsePointLights(pointLights, pointLightCount);

		shaderList[1]->UseMaterial(model->GetMaterial());
	}

	glUniform1i(loc_diffuseSampler, 0);
	glUniform1i(loc_normalSampler, 1);

	
	model->RenderModel();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	if (hitbox)
	{
		//텍스처 중복 문제 해결
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
	}

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}