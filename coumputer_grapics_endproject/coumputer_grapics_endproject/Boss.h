#pragma once

#include "gl/glm/glm.hpp"
#include <map>
#include <iostream>

class Model;
class Collision;
class Animation;
class Animator;
class Terrain;
class CameraBase;
class DirectionalLight;
class PointLight;
class Object;
class Player;
class BossBehavior;

class Boss
{
public:
	Boss(Model* model, Model* hitbox, Model* SlamEffect, Player* player, std::map<std::pair<int, int>, Object*> map);
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
	Model* SlamEffect;
	Collision* collisionbox;
	Animator* animator;
	Player* player;
	BossBehavior* behavior;

	Animation* idleAnim;
	Animation* walkAnim;
	Animation* runAnim;
	Animation* fastrunAnim;
	Animation* atkAnim;
	Animation* jumpAtkAnim;
	Animation* deadAnim;

	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float upwardSpeed;
	float groundHeight;
	float time_paturn{};

	bool isJumping;
	bool inRange;

	std::map<std::pair<int, int>, Object*> map;
};
