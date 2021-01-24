#pragma once

#include "Example.h"

class Triangle : public Example
{

public:

	Triangle();
	~Triangle();

	void Init() override;
	void Release() override;
	void Update() override;
};