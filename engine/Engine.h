#pragma once

#include "NonCopyable.h"

class Example;

class Engine : public NonCopyable
{

public:

	Engine(Example* example);
	~Engine();

	void Init();
	void Release();
	void Update();

protected:

	

private:

	

protected:

	

private:

	Example* m_Example;
};