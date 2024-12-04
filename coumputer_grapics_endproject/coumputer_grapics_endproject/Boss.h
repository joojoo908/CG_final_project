#pragma once

#include "gl/glm/glm.hpp"

class Model;
class Animation;
class Animator;
class Terrain;
class CameraBase;
class DirectionalLight;
class PointLight;

class Boss
{
public:
	Boss(Model* model);
	bool Move(float deltaTime);
	void update(float deltaTime);
	void draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount);
	float GetRotY();

	Model* GetModel() { return model; }
	Animator* GetAnimator() { return animator; }

private:
	void Jump();

	Model* model;
	Animator* animator;

	Animation* idleAnim;
	Animation* danceAnim;
	Animation* runAnim;

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed_x;
	float currMoveSpeed_z;
	float upwardSpeed;
	float groundHeight;

	bool isJumping;
};
