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
	Player(Model* model, Model* hitbox, std::map<std::pair<int, int>, Object*> map);
	void HandleInput(unsigned char keys, bool updown, float deltaTime);
	bool Move(float deltaTime, std::map<std::pair<int, int>, Object*> map);
	void MouseContrl(float XChange, float YChange);
	void update(float deltaTime, std::map<std::pair<int, int>, Object*> map);
	void draw(CameraBase* currCamera, DirectionalLight* directionalLight, PointLight* pointLights[], unsigned int pointLightCount);
	

	float GetRotY();
	void GetDamage(float Damage);
	int GetKey() { return LightKey; }
	float GetHp() { return HP; };
	void SetKey() { LightKey = 5; }
	bool IsRoll() { return  is_Roll; }
	bool iswork() { return  is_Working; }
	bool IS_END() { return  is_Live && is_End; }
	bool IS_DEAD() { return !is_Live && is_End; }
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
	Animation* jumpAnim;
	Animation* rollAnim;
	Animation* deathAnim;
	Animation* sitAnim;
	Animation* danceAnim;


	std::map<std::pair<int, int>, Object*> map;
	const float MOVE_SPEED;
	const float TURN_SPEED;
	const float GRAVITY;
	const float JUMP_POWER;

	float currMoveSpeed_x;
	float currMoveSpeed_z;
	float upwardSpeed;
	float groundHeight;
	float HP;
	float working_time;
	float rolling_time;
	float block_rolltime;
	float ending_time;
	float dance_time;

	bool isJumping;
	bool is_Working;
	bool is_Roll;
	bool is_Live;
	bool is_Clear[4];
	bool can_escape;
	bool is_End;

	int LightKey{};

	bool InRange(int x, int z, int goal);
	bool InRange(const std::pair<int, int>& a, int distance);
	bool isMachine(int x, int z);
	bool Collide(Collision* collision, glm::vec3 delta);
	void UpdateHitbox();
	void OpenEnding();
	void Ending();
	void Rolling(float deltaTime);
	
};
