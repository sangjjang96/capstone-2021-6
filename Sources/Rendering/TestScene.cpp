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
	scene->CreateEntity("sponza");
	scene->CreateEntity("backpack");
	scene->CreateLight();
	scene->CreateCamera();

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.09f));

	m_Model = new Model("../Resources/resources/objects/Sponza-master/sponza.obj");
	scene->GetEntities()[0]->SetModel(m_Model);
	scene->GetEntities()[0]->SetTransform(model);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));

	m_Model = new Model("../Resources/resources/objects/backpack/backpack.obj");
	scene->GetEntities()[1]->SetModel(m_Model);
	scene->GetEntities()[1]->SetTransform(model);

	scene->GetLights()[0]->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	scene->GetLights()[0]->SetColor(glm::vec3(300.0f, 190.0f, 100.0f));
	scene->GetCameras()[0]->SetPos(glm::vec3(0.0f, 3.0f, 5.0f));
}