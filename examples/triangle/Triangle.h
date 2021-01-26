#pragma once

#include "Example.h"

class Triangle : public Example
{

public:

	Triangle();
	~Triangle();

	void ConfigDeviceProperties() override;
	void Init() override;
	void Release() override;
	void Update() override;
};