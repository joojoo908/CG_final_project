#pragma once
#include "gl/glm/glm.hpp"
#include <map>
#include <iostream>

class Model;
class Boss;
class Player;

class BossBehavior
{
public:
	BossBehavior(Model* bossmodel, Model* playermodel, std::map<std::pair<int, int>, Object*> map);
	

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
	int GetKey() { return key; }
	Model* GetBossModel() { return model_b; }

private:
	Model* model_b; //--- ���� ��
	Model* model_p; //--- �÷��̾� ��
	const float MOVE_SPEED;

	std::map<std::pair<int, int>, Object*> map;
	int key{};		//--- �ൿ Ű
	float turning_time[5]{}; //--- Ű ���� Ÿ��
	bool InRange(int distance);
};
         
