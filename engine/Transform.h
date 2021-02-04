#pragma once

#include "Env.h"

class Transform
{

public:

	Transform();
	virtual ~Transform();

	glm::mat4& GetMatrix();
	void SetMatrix(glm::mat4& m);

	Transform& Scale(float x, float y, float z);
	Transform& Rotate(float radian, float x, float y, float z);
	Transform& Translate(float x, float y, float z);

private:

	glm::mat4 m_Matrix;
};