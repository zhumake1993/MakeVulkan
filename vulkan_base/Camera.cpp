#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::LookAt(glm::vec3 pos, glm::vec3 target)
{
	m_Position = pos;
	m_Look = target - pos;
}

void Camera::SetLens(float fovY, float aspect, float nearZ, float farZ)
{
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = nearZ;
	m_FarZ = farZ;
}

void Camera::Strafe(float d)
{
	auto right = glm::cross(m_WorldUp, m_Look);
	m_Position += right * d;
}

void Camera::Walk(float d)
{
	m_Position += m_Look * d;
}

void Camera::Fly(float d)
{
	m_Position += m_WorldUp * d;
}

void Camera::Pitch(float angle)
{
	auto right = glm::cross(m_WorldUp, m_Look);
	auto mat = glm::rotate(glm::mat4(1.0f), angle, right);
	m_Look = mat * glm::vec4(m_Look, 0.0f);
}

void Camera::RotateY(float angle)
{
	auto mat = glm::rotate(glm::mat4(1.0f), angle, m_WorldUp);
	m_Look = mat * glm::vec4(m_Look, 0.0f);
}

glm::mat4 Camera::GetView()
{
	auto right = glm::cross(m_WorldUp, m_Look);
	auto up = glm::cross(m_Look, right);
	return glm::lookAt(m_Position, m_Position + m_Look, up);
}

glm::mat4 Camera::GetProj()
{
	return glm::perspective(m_FovY, m_Aspect, m_NearZ, m_FarZ);
}