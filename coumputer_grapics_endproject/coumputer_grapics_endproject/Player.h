#pragma once
#include <iostream>
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
	void GetDamage(float Damage);

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
	Animation* runAnim;
	Animation* backRunAnim;
	Animation* leftRunAnim;
	Animation* rightRunAnim;
	Animation* sitAnim;

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed_x;
	float currMoveSpeed_z;
	float upwardSpeed;
	float groundHeight;
	float HP;

	bool isJumping;
	bool InRange(int x,int z);
	bool InRange(const std::pair<int, int>& a, int distance);
	bool Collide(Collision* collision, glm::vec3 delta);
	void UpdateHitbox();
};
