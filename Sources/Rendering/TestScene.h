#pragma once
#include "Scene.h"

class Entity;
class Light;
class Model;
class TestScene : public Scene
{
public:
	TestScene() :
		m_mainEntity(nullptr),
		m_mainLight(nullptr),
		m_backpack(nullptr)
	{
	}

	void Init() override;

private:
	Entity* m_mainEntity;
	Light* m_mainLight;
	Model* m_backpack;
};