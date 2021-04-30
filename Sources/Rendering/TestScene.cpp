#include "TestScene.h"
#include "Scene.h"
#include "Entity.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "Renderer.h"

#include <GLFW/glfw3.h>

#include <iostream>

void TestScene::Init()
{
	Scene* scene = this;
	m_Sponza = &scene->CreateEntity("sponza");
	m_Backpack = &scene->CreateEntity("backpack");
	m_mainLight = &scene->CreateLight();
	m_mainCamera = &scene->CreateCamera();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.09f));

	m_Model = new Model("../Resources/resources/objects/Sponza-master/sponza.obj");
	m_Sponza->SetModel(m_Model);
	m_Sponza->SetTransform(model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));

	m_Model = new Model("../Resources/resources/objects/backpack/backpack.obj");
	m_Backpack->SetModel(m_Model);
	m_Backpack->SetTransform(model);

	m_mainLight->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	m_mainLight->SetColor(glm::vec3(300.0f, 190.0f, 100.0f));
	m_mainCamera->SetPos(glm::vec3(0.0f, 3.0f, 5.0f));
}