#include "FreeCamera.h"

#include <gl/glm/glm.hpp>
#include <gl/glm\gtc\matrix_transform.hpp>
#include <gl/glm\gtc\type_ptr.hpp>

//#include "Window.h"

FreeCamera::FreeCamera(glm::vec3 startPosition, GLfloat moveSpeed, GLfloat turnSpeed,
	GLfloat startPitch, GLfloat startYaw) : CameraBase(startPosition, startPitch, startYaw)
{
	this->moveSpeed = moveSpeed;
	this->turnSpeed = turnSpeed;

	canMove = false;
}


void FreeCamera::KeyControl(unsigned char keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;

    if (keys == 'w') {
        position += front * velocity; // WŰ�� ������ �� �������� �̵�
    }
    if (keys == 's') {
        position -= front * velocity; // SŰ�� ������ �� �Ĺ����� �̵�
    }
    if (keys == 'a') {
        position -= right * velocity; // AŰ�� ������ �� �������� �̵�
    }
    if (keys == 'd') {
        position += right * velocity; // DŰ�� ������ �� �������� �̵�
    }
    if (keys == 'e') {
        position += up * velocity; // �����̽��ٸ� ������ �� ���� �̵�
    }
    if (keys == 'q') { // Shift Ű
        position -= up * velocity; // Shift Ű�� ������ �� �Ʒ��� �̵�
    }
}

void FreeCamera::MouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f)
		pitch = 89.0f;

	if (pitch < -89.0f)
		pitch = -89.0f;
}

void FreeCamera::ScrollControl(GLfloat scrollY)
{
	moveSpeed += scrollY * 5.f;
	moveSpeed = std::max(0.1f, moveSpeed);
}

FreeCamera::~FreeCamera()
{
}
