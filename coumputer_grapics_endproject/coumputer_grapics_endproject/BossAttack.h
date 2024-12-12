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
class Player;
class Boss;

class BossAttack
{
public:
	BossAttack(Boss* boss);
	int GetStatus() { return status; }
	Boss* GetBoss() { return boss; }
	void Slam();
	void closeAttack();
	void Dash();
	void Wander();
	
	 
private:
	Boss* boss;
	int status;
	//bool Slam; //--내려찍는 스킬
	//bool closeAttack; // 기본 근접 공격
	////bool closeAttack;
	//bool Dash; // 플레이어의 위치로 대쉬
	
};

