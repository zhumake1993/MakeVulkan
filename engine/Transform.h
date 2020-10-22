#pragma once

#include "Common.h"

class Transform
{

public:

	Transform();
	~Transform();

	glm::mat4 GetMatrix();
	void SetMatrix(glm::mat4& m);

	glm::vec3 GetUp();

	Transform& Scale(float x, float y, float z);
	Transform& Rotate(float radian, float x, float y, float z);
	Transform& Translate(float x, float y, float z);

	Transform& RotateLocal(float radian);

private:

	glm::mat4 m_Matrix;
};