#pragma once
#include <GL/freeglut.h>
#include <gl/GLM/glm.hpp>
#include <gl/GLM/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);


	void keyControl(unsigned char key, int x, int y); // Ű���� �Է� ó��
	void specialKeyControl(int key, int x, int y); // Ư��Ű �Է� ó��
	void mouseControl(int x, int y); // ���콺 �Է� ó��
	void mouseClick(int button, int state, int x, int y);
	glm::mat4 calculateViewMatrix();

private:
	glm::vec3 position; // ī�޶� ��ġ
	glm::vec3 front; // ī�޶� �ٶ󺸴� ����
	glm::vec3 up; // ī�޶� ���
	glm::vec3 right; // ī�޶� ������
	glm::vec3 worldUp; // ���忡���� �� ����

	GLfloat yaw; // ���� ȸ��
	GLfloat pitch; // ���� ȸ��

	GLfloat moveSpeed; // �̵� �ӵ�
	GLfloat turnSpeed; // ȸ�� �ӵ�

	// ī�޶� ��ǥ�� ������Ʈ
	void update();
	bool isMousePressed;

	// ���콺 ��ġ ����
	GLfloat lastX, lastY;
	GLfloat xChange, yChange;
};