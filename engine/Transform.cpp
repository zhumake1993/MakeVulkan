#include "Transform.h"

Transform::Transform()
{
	m_Matrix = glm::mat4(1.0f);
}

Transform::~Transform()
{
}

glm::mat4& Transform::GetMatrix()
{
	return m_Matrix;
}

void Transform::SetMatrix(glm::mat4& m)
{
	m_Matrix = m;
}

glm::vec3 Transform::GetPosition()
{
	return m_Matrix[3];
}

Transform& Transform::Scale(float x, float y, float z)
{
	m_Matrix = glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z)) * m_Matrix;

	return *this;
}

Transform& Transform::Rotate(float radian, float x, float y, float z)
{
	// 不能写成 m_Matrix = glm::rotate(m_Matrix, radian, glm::vec3(x, y, z)); 似乎跟GLM_FORCE_LEFT_HANDED有关？
	m_Matrix = glm::rotate(glm::mat4(1.0f), radian, glm::vec3(x, y, z)) * m_Matrix;

	return *this;
}

Transform & Transform::RotateLocal(float radian, float x, float y, float z)
{
	// 参考：https://stackoverflow.com/questions/21923482/rotate-and-translate-object-in-local-and-global-orientation-using-glm
	m_Matrix = m_Matrix * glm::rotate(glm::mat4(1.0f), radian, glm::vec3(x, y, z));

	return *this;
}

Transform& Transform::Translate(float x, float y, float z)
{
	m_Matrix = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)) * m_Matrix;

	return *this;
}