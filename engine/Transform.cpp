#include "Transform.h"

Transform::Transform()
{
	m_Matrix = glm::mat4(1.0f);
}

Transform::~Transform()
{
}

glm::mat4 Transform::GetMatrix()
{
	return m_Matrix;
}

void Transform::SetMatrix(glm::mat4& m)
{
	m_Matrix = m;
}

glm::vec3 Transform::GetUp()
{
	return glm::vec3(m_Matrix[1][0], m_Matrix[1][1], m_Matrix[1][2]);
}

Transform& Transform::Scale(float x, float y, float z)
{
	m_Matrix = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z)) * m_Matrix;

	return *this;
}

Transform& Transform::Rotate(float radian, float x, float y, float z)
{
	m_Matrix = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(x, y, z)) * m_Matrix;

	return *this;
}

Transform& Transform::Translate(float x, float y, float z)
{
	m_Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * m_Matrix;

	return *this;
}

Transform & Transform::RotateLocal(float radian)
{
	m_Matrix = glm::rotate(glm::mat4(1.0f), radian, GetUp()) * m_Matrix;

	return *this;
}
