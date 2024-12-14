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
	BossBehavior(Model* bossmodel, Model* playermodel, Model* SlamEffect, Collision* boss_c, Collision* player_c, std::map<std::pair<int, int>, Object*> map);
		
	void Wander(float deltaTime);
	void Chase(float deltaTime);
	void closeAttack();
	void Dash(float deltaTime);
	void Slam(float deltaTime);
	void Do(float deltaTime);

	void Turn_to_Player();
	void Check_Paturn();
	void Update(float deltaTime);

	void updateSlam();
	void GetSlam();
	bool is_SLAM() { return isSLAM; }

	float GetDamage() { return Damage; }
	void SetDamage(float Damage) { this->Damage = Damage;}
	void UpdateHitbox();
	
	int GetKey() { return key; }
	Model* GetBossModel() { return model_b; }
	Collision* GetCollsion() { return box_b; }

private:
	Model* model_b; //--- ���� ��
	Model* model_p; //--- �÷��̾� ��
	Model* SlamEffect;

	Collision* box_b;	//--- ���� ��;
	Collision* box_p;	//--- �÷��̾� ��

	const float WALK_SPEED;
	const float RUN_SPEED;
	const float DASH_SPEED;
	const float SLAM_SPEED;
	bool isSLAM;
	bool Dmg_SLAM;
	bool Dmg_Close_atk;

	float Damage{};


	std::map<std::pair<int, int>, Object*> map;
	int key{};		//--- �ൿ Ű
	float turning_time{}; //--- Ű ���� Ÿ��
	int skill_order{};
	bool InRange_Slam();
	bool InRange(int distance);
	bool Collide(Collision* collision, glm::vec3 delta);
};

         
