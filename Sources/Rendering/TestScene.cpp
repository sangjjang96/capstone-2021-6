#include "TestScene.h"
#include "Scene.h"
#include "Entity.h"
#include "Model.h"
#include "Light.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

#include <iostream>

void TestScene::Init()
{
	m_mainEntity = &CreateEntity("backpack");
	m_backpack = new Model("../Resources/resources/objects/backpack/backpack.obj");
	m_mainEntity->SetModel(m_backpack);

	m_mainLight = &CreateLight();
	m_mainLight->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	m_mainLight->SetColor(glm::vec3(30.0f, 19.0f, 10.0f));
}