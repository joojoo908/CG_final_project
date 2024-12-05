#pragma once
#include "gl/glm/glm.hpp"

class Model;

class Collision
{
public:
	Collision(Model* model);
	void NextPosition(glm::vec3 delta);
	void Update();

	~Collision();
	float minX;
	float maxX;
	float minY;
	float maxY;
	float minZ;
	float maxZ;
private:
    Model* model;

};
