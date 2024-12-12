#pragma once

#include "gl/glm/glm.hpp"
#include <map>

class Model;
class Collision;
class Animation;
class Animator;
class Terrain;
class CameraBase;
class DirectionalLight;
class PointLight;
class Object;

class Player
{
public:
	Player(Model* model, Model* hitbox);
	void HandleInput(unsigned char keys, bool updown, float deltaTime);
	bool Move(float deltaTime, std::map<std::pair<int, int>, Object*> map);
	void MouseContrl(float XChange, float YChange);
	void update(float deltaTime, std::map<std::pair<int, int>, Object*> map);
	void draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount);
	float GetRotY();
	

	Model* GetModel() { return model; }
	Animator* GetAnimator() { return animator; }
	Collision* GetCollsion() { return collisionbox; }
	

private:
	void Jump();

	Model* model;
	Model* hitbox;
	Collision* collisionbox;
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
	bool InRange(int x1,int z1, int x2, int z2);
	bool InRange(const std::pair<int, int>& a, int distance);
	bool Collide(Collision* collision, glm::vec3 delta);
	void UpdateHitbox();
};
