#include "Camera.h"

// �⺻ ������
Camera::Camera()
    : position(glm::vec3(0.0f, 0.0f, 3.0f)), front(glm::vec3(0.0f, 0.0f, -1000.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f)), right(glm::vec3(1.0f, 0.0f, 0.0f)),
    worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), yaw(-90.0f), pitch(0.0f),
    moveSpeed(2.5f), turnSpeed(0.1f), lastX(0.0f), lastY(0.0f), xChange(0.0f), yChange(0.0f)
{
    update();
}

// ������
Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
    : position(startPosition), up(startUp), yaw(startYaw), pitch(startPitch),
    moveSpeed(startMoveSpeed), turnSpeed(startTurnSpeed), lastX(0.0f), lastY(0.0f), xChange(0.0f), yChange(0.0f)
{
    front = glm::vec3(0.0f, 0.0f, -1.0f); // �⺻������ ī�޶�� -z ������ ��
    right = glm::normalize(glm::cross(up, front)); // ī�޶��� ������ ���� ���
    worldUp = startUp; // ���忡���� �� ����
    update();
}

// Ű���� �Է� ó��
void Camera::keyControl(unsigned char key, int x, int y)
{
    GLfloat velocity = moveSpeed * 0.1f; // �̵� �ӵ� ���� (������ ����)

    if (key == 27) { // ESC Ű�� ������ �� â�� �ݵ��� ����
        exit(0);
    }

    if (key == 'w') {
        position += front * velocity; // WŰ�� ������ �� �������� �̵�
    }
    if (key == 's') {
        position -= front * velocity; // SŰ�� ������ �� �Ĺ����� �̵�
    }
    if (key == 'a') {
        position -= right * velocity; // AŰ�� ������ �� �������� �̵�
    }
    if (key == 'd') {
        position += right * velocity; // DŰ�� ������ �� �������� �̵�
    }
    if (key == ' ') {
        position += up * velocity; // �����̽��ٸ� ������ �� ���� �̵�
    }
    if (key == 0x10) { // Shift Ű
        position -= up * velocity; // Shift Ű�� ������ �� �Ʒ��� �̵�
    }

    update();
}

// Ư�� Ű �Է� ó�� (��: ����Ű)
void Camera::specialKeyControl(int key, int x, int y)
{
    // Ư��Ű ó�� (��: ����Ű)
    if (key == GLUT_KEY_F1) {
        // F1 Ű�� ������ �� ó���� ����
    }
}

void Camera::mouseClick(int button, int state, int x, int y)
{
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        isMousePressed = false;
    }
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        isMousePressed = true;
        lastX = x;
        lastY = y;
    }
}

// ���콺 �Է� ó��
void Camera::mouseControl(int x, int y)
{
    if (isMousePressed) {

        GLfloat xOffset = x - lastX;
        GLfloat yOffset = lastY - y; // ���콺 Y���� �ݴ�� ���� (ȭ�� ��ǥ��� ���߱� ����)

        lastX = x;
        lastY = y;

        xChange = xOffset;
        yChange = yOffset;

        yaw += xChange * turnSpeed; // ���콺 X �̵��� ���� Yaw ȸ��
        pitch -= yChange * turnSpeed; // ���콺 Y �̵��� ���� Pitch ȸ��

        // ��ġ ���� (���� �ʹ� �ø��ų� ������ �ʵ���)
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

    }

    update();
}

// ī�޶� ��ǥ�踦 ������Ʈ
void Camera::update()
{
    // ī�޶��� �ٶ󺸴� ���� ���͸� ���
    glm::vec3 frontTemp;
    frontTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontTemp.y = sin(glm::radians(pitch));
    frontTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontTemp);

    // ������ ���� ���
    right = glm::normalize(glm::cross(front, worldUp));

    // �� ���� ���� ���
    up = glm::normalize(glm::cross(right, front));
}

// ī�޶��� �� ��� ���
glm::mat4 Camera::calculateViewMatrix()
{
    return glm::lookAt(position, position + front, up); // ī�޶��� ��ġ, �ٶ󺸴� ����, �� ����
}