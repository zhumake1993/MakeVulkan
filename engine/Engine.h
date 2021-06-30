#pragma once

#include "NonCopyable.h"

class TimeManager;
class RendererScene;
class Imgui;

class Engine : public NonCopyable
{
public:

	Engine();
	virtual ~Engine();

	void InitEngine();
	void ReleaseEngine();
	void UpdateEngine();

protected:

	virtual void PreInit() = 0;
	virtual void Init() = 0;
	virtual void Release() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

private:

	

protected:

	TimeManager* m_TimeManager = nullptr;
	RendererScene* m_RendererScene = nullptr;
	Imgui* m_Imgui = nullptr;

private:

	
};