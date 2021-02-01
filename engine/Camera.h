#pragma once

#include "Env.h"

class Camera
{

public:

	Camera();
	~Camera();

	glm::vec3 GetPosition();
	void LookAt(glm::vec3 pos, glm::vec3 target);
	void SetLens(float fovY, float aspect, float nearZ, float farZ);
	void SetAspect(float aspect);
	void SetSpeed(float moveSpeed, float rotateSpeed);
	void Update(float deltaTime);

	// ��3������û�õ�������
	// ���ڵ�ʵ�ַ�����б���ƶ����ٶȱ���m_MoveSpeed
	void Strafe(float d);
	void Walk(float d);
	void Fly(float d);
	void Pitch(float angle);
	void RotateY(float angle);

	glm::mat4 GetView();
	glm::mat4 GetProj();

private:

	// Լ����"��"����
	glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 m_Look = glm::vec3(0.0f, 0.0f, 1.0f);

	float m_FovY = 0.0f;
	float m_Aspect = 0.0f;
	float m_NearZ = 0.0f;
	float m_FarZ = 0.0f;

	float m_MoveSpeed;
	float m_RotateSpeed;
};