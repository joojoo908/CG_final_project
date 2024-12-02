#include "Object.h"

#include <iostream>
#include <GL/glew.h>
#include "Animation.h"
#include "Animator.h"
#include "Model.h"
#include "ShaderHandle.h"

#include "CameraBase.h"
#include "DirectionalLight.h"
#include "PointLight.h"
//#include "Terrain.h"

Object::Object(Model *model, Animator* animator = NULL , float x=0, float z=0) : GRAVITY(0.2f)
{
	this->model = new Model(*model);
	//this->model->LoadModel(modelPath);
	if (!animator) {
		std::cout << "null!!!" << std::endl;
	}
	else {
		this->animator = animator;
	}

	glm::vec3 newTns3(x, 0, z);
	this->model->SetTranslate(newTns3);

	GLfloat* currRot = this->model->GetRotate();
	float rotation = 180;
	float newRotx = currRot[0] + rotation;
	glm::vec3 newRot2(newRotx, currRot[1], currRot[2]);
	this->model->SetRotate(newRot2);

}


float Object::GetRotY()
{
	return model->GetRotate()[1];
}


void Object::update(float deltaTime) {

	animator->UpdateAnimation(deltaTime);

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
	//텍스처 중복 문제 해결
	glBindTexture(GL_TEXTURE_2D, 0);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		std::cout << "error : " << error << std::endl;
	}
}