#pragma once

#include "gl/glm/glm.hpp"

class Model;
class Animation;
class Animator;
class Terrain;
class CameraBase;
class DirectionalLight;
class PointLight;

class Player
{
public:
	Player(Model* model, Model* hitbox);
	void HandleInput(unsigned char keys, bool updown, float deltaTime);
	bool Move(float deltaTime);
	void MouseContrl(float XChange, float YChange);
	void update(float deltaTime);
	void draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount);
	float GetRotY();
	void UpdateHitbox();
	Model* GetModel() { return model; }
	Animator* GetAnimator() { return animator; }

private:
	void Jump();

	Model* model;
	Model* hitbox;
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
