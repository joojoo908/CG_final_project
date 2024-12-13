#pragma once
#include "gl/glm/glm.hpp"
#include <map>
#include <iostream>

class Model;
class Boss;
class Player;
class Collision;
class Object;

class BossBehavior
{
public:
	BossBehavior(Model* bossmodel, Model* playermodel, Collision* boss_c, Collision* player_c, std::map<std::pair<int, int>, Object*> map);
		
	void Wander(float deltaTime);
	void Chase(float deltaTime);
	void closeAttack();
	void Dash();
	void Slam();
	void Do(float deltaTime);

	void Turn_to_Player();
	void Check_Paturn();
	void SetKey(int goal);
	void Update(float deltaTime);

	void reset_time();
	void UpdateHitbox();
	
	int GetKey() { return key; }
	Model* GetBossModel() { return model_b; }
	Collision* GetCollsion() { return box_b; }

private:
	Model* model_b; //--- 보스 모델
	Model* model_p; //--- 플레이어 모델
	Collision* box_b;	//--- 보스 모델;
	Collision* box_p;	//--- 플레이어 모델


	const float MOVE_SPEED;

	std::map<std::pair<int, int>, Object*> map;
	int key{};		//--- 행동 키
	float turning_time{}; //--- 키 변경 타임
	bool InRange(int distance);
	bool Collide(Collision* collision, glm::vec3 delta);
};

         
