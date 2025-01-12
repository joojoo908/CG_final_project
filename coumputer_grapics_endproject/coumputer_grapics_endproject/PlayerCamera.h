#pragma once
#include "CameraBase.h"

class Player;

class PlayerCamera : public CameraBase
{
public:
	PlayerCamera(Player* player);

	virtual void Update() override;
	virtual void KeyControl(unsigned char keys, GLfloat deltaTime) override;
	virtual void MouseControl(GLfloat xChange, GLfloat yChange) override;
	virtual void ScrollControl(GLfloat scrollY) override;

private:
	float CalcHorizontalDistance();
	float CalcVerticalDistance();
	void CalcPosition(float horizontalDistance, float verticalDistance);

	Player* player;
	float distance;
	float angle;
	bool canMove;
	float TURN_SPEED;
	const float HEIGHT_OFFSET;
};

