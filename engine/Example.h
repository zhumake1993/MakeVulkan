#pragma once

#include "NonCopyable.h"

class Example : public NonCopyable
{
public:

	Example() {}
	~Example() {}

	virtual void Init() = 0;
	virtual void Release() = 0;
	virtual void Update() = 0;
};