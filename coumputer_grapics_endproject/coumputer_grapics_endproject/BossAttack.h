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
	//bool Slam; //--������� ��ų
	//bool closeAttack; // �⺻ ���� ����
	////bool closeAttack;
	//bool Dash; // �÷��̾��� ��ġ�� �뽬
	
};

