#pragma once
#include "Scene.h"
#include <glm/glm.hpp>

class Entity;
class Model;
class TestScene : public Scene
{
public:
	TestScene() :
		m_Model(nullptr),
		Scene()
	{
	}

	virtual void Init() override;

private:
	Model* m_Model;
};