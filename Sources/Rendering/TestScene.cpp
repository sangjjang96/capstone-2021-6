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
	m_mainEntity = &CreateEntity("sponza");
	m_sponza = new Model("../Resources/resources/objects/Sponza-master/sponza.obj");
	m_mainEntity->SetModel(m_sponza);

	m_mainLight = &CreateLight();
	m_mainLight->SetPosition(glm::vec3(0.0f, 200.0f, 0.0f));
	m_mainLight->SetColor(glm::vec3(300.0f, 190.0f, 100.0f));
}