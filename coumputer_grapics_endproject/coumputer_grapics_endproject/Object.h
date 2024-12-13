#pragma once
#pragma once

#include "gl/glm/glm.hpp"
#include <string>
#include <map>

class Model;
class Collision;
class Animation;
class Animator;
class Terrain;
class CameraBase;
class DirectionalLight;
class PointLight;

class Object
{
public:
	Object(std::string type,Model* model, Model* hitbox, Animator* animator, float x, float z, bool bilboard);
	void update(float deltaTime, glm::vec3 v);
	void draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount);
	float GetRotY();
	void update_pannel(CameraBase* currCamera);

	Model* GetModel() { return model; }
	Animator* GetAnimator() { return animator; }
	Collision* GetCollision() { return collisionbox; }
	Model* hitbox;
private:
	std::string type;
	bool bilboard;

	Model* model;
	Collision* collisionbox;
	Animator* animator;

	Animation* ani;
	
	const float GRAVITY;
	float groundHeight;
	
};

