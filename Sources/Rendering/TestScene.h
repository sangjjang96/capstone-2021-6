#pragma once
#include "Scene.h"
#include <glm/glm.hpp>

class Entity;
class Light;
class Camera;
class Model;
class TestScene : public Scene
{
public:
	TestScene() :
		m_Model(nullptr),
		m_Sponza(nullptr),
		m_Backpack(nullptr),
		m_mainLight(nullptr),
		m_mainCamera(nullptr),
		Scene()
	{
	}

	virtual void Init() override;

private:
	Model* m_Model;
	Entity* m_Sponza;
	Entity* m_Backpack;
	Light* m_mainLight;
	Camera* m_mainCamera;
};