#include "PlayerCamera.h"

#include <iostream>

#include "Player.h"
#include "Model.h"

PlayerCamera::PlayerCamera(Player* player) : CameraBase(glm::vec3(0.f, 0.f, 0.f)), HEIGHT_OFFSET(1.f)
{
	this->player = player;
	canMove = false;

	distance = 3.f;
	angle = 0.f;
	pitch = -15.f;
	TURN_SPEED = 0.5f;
	farClippingPlane = 60.0f;
	Update();
}

void PlayerCamera::Update()
{
	float horizontalDistance = CalcHorizontalDistance();
	float verticalDistance = CalcVerticalDistance();
	CalcPosition(horizontalDistance, verticalDistance);
	yaw = 90.f - player->GetRotY();

	CameraBase::Update();
}

void PlayerCamera::KeyControl(unsigned char keys, GLfloat deltaTime)
{
	return;
}

void PlayerCamera::MouseControl(GLfloat xChange, GLfloat yChange)
{
	pitch += yChange * TURN_SPEED;;
	if (pitch > 50.f)
		pitch = 50.f;

	if (pitch < -70.0f)
		pitch = -70.0f;
	if (pitch >= 15)
	{
		distance = 0.05 * (65.f - pitch) + 0.2f;
	}
	else
	{
		distance = 3;
	}
}

void PlayerCamera::ScrollControl(GLfloat scrollY)
{
	distance -= scrollY;
}

float PlayerCamera::CalcHorizontalDistance()
{
	return distance * cosf(glm::radians(-pitch));
}

float PlayerCamera::CalcVerticalDistance()
{
	return distance * sinf(glm::radians(-pitch));
}

void PlayerCamera::CalcPosition(float horizontalDistance, float verticalDistance)
{
	GLfloat* playerPos = player->GetModel()->GetTranslate();
	position.y = playerPos[1] + verticalDistance + HEIGHT_OFFSET;

	float theta = player->GetRotY() + angle;
	float offsetX = horizontalDistance * sinf(glm::radians(theta));
	float offsetZ = horizontalDistance * cosf(glm::radians(theta));

	position.x = playerPos[0] - offsetX;
	position.z = playerPos[2] - offsetZ;
}
