#include <iostream>
#include <vector>
#include <string>

#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "Scene.h"
#include "Entity.h"
#include "Model.h"
#include "Light.h"
#include "GBuffer.h"

Renderer::Renderer() :
	m_winWidth(0),
	m_winHeight(0),

	m_shadowWidth(0),
	m_shadowHeight(0),

	m_quadVAO(0),
	m_quadVBO(0),

	m_depthMapFBO(0),
	m_depthCubemap(0),

	m_entityNum(0),
	m_lightNum(0),

	m_bShadows(false),

	m_geometryShader(nullptr),
	m_lightShader(nullptr),
	m_shadowShader(nullptr),

	m_GBuffer(nullptr)
{
}

Renderer::~Renderer()
{
	if (m_geometryShader != nullptr)
	{
		delete m_geometryShader;
		m_geometryShader = nullptr;
	}

	if (m_lightShader != nullptr)
	{
		delete m_lightShader;
		m_lightShader = nullptr;
	}

	if (m_shadowShader != nullptr)
	{
		delete m_shadowShader;
		m_shadowShader = nullptr;
	}

	if (m_GBuffer != nullptr)
	{
		delete m_GBuffer;
		m_GBuffer = nullptr;
	}
}

// Set up Framebuffer object, Shader, Main Object and Screen Fill Quad
void Renderer::StartRenderer(unsigned int width, unsigned int height)
{
	// Set Parameters
	m_winWidth = width;
	m_winHeight = height;

	m_geometryShader = new Shader("../Resources/Shaders/GeometryPass.vs", "../Resources/Shaders/GeometryPass.fs");
	m_lightShader = new Shader("../Resources/Shaders/LightPass.vs", "../Resources/Shaders/LightPass.fs");
	m_shadowShader = new Shader("../Resources/Shaders/Shadow.vs", "../Resources/Shaders/Shadow.fs", "../Resources/Shaders/Shadow.gs");

	m_GBuffer = new GBuffer(width, height);
	m_GBuffer->Init();

	// Set Quad
	float quadVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);
	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// Set Shadow
	const unsigned int shadowWidth = 5120, shadowHeight = 5120;
	m_shadowWidth = shadowWidth;
	m_shadowHeight = shadowHeight;

	glGenFramebuffers(1, &m_depthMapFBO);

	glGenTextures(1, &m_depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, m_shadowWidth, m_shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_lightShader->UseProgram();
	m_lightShader->SetInt("posData", 0);
	m_lightShader->SetInt("normalData", 1);
	m_lightShader->SetInt("albedoData", 2);
	m_lightShader->SetInt("depthMap", 3);
}

void Renderer::GeometryPass(Scene& scene)
{
	// Set Geometry Pass
	glViewport(0, 0, m_winWidth, m_winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_GBuffer->SetFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (unsigned int i = 0; i < scene.GetEntities().size(); ++i)
	{
		glm::mat4 projection = glm::perspective(glm::radians(scene.GetCameras()[0]->m_ZOOM), (float)m_winWidth / (float)m_winHeight, 0.1f, 2000.0f);
		glm::mat4 view = scene.GetCameras()[0]->GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		m_geometryShader->UseProgram();
		m_geometryShader->SetMat4("projMatrix", projection);
		m_geometryShader->SetMat4("viewMatrix", view);

		scene.GetEntities()[i]->Render(*m_geometryShader);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::ShadowPass(Scene& scene)
{
	// Set Shadow Pass
	float near_plane = 0.1f, far_plane = 2000.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)m_shadowWidth / (float)m_shadowHeight, near_plane, far_plane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(scene.GetLights()[0]->GetPosition(), scene.GetLights()[0]->GetPosition() + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(scene.GetLights()[0]->GetPosition(), scene.GetLights()[0]->GetPosition() + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(scene.GetLights()[0]->GetPosition(), scene.GetLights()[0]->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(scene.GetLights()[0]->GetPosition(), scene.GetLights()[0]->GetPosition() + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(scene.GetLights()[0]->GetPosition(), scene.GetLights()[0]->GetPosition() + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(scene.GetLights()[0]->GetPosition(), scene.GetLights()[0]->GetPosition() + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

	glViewport(0, 0, m_shadowWidth, m_shadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	m_shadowShader->UseProgram();
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_shadowShader->SetMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
	}
	m_shadowShader->SetFloat("far_plane", far_plane);
	m_shadowShader->SetVec3("lightPos", scene.GetLights()[0]->GetPosition());

	for (unsigned int i = 0; i < scene.GetEntities().size(); ++i)
		scene.GetEntities()[i]->Render(*m_shadowShader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::LightPass(Scene& scene)
{
	// Set Light Pass
	glViewport(0, 0, m_winWidth, m_winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_lightShader->UseProgram();
	m_GBuffer->SetTexture();
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubemap);

	m_lightShader->SetVec3("camPos", scene.GetCameras()[0]->GetPos());
	m_lightShader->SetFloat("far_plane", 100.0f);
	m_lightShader->SetFloat("metallic", 0.4f);
	m_lightShader->SetFloat("roughness", 0.4f);
	m_lightShader->SetFloat("ao", 1.0f);
	m_lightShader->SetBool("b_shadows", m_bShadows);

	glm::mat4 model = glm::mat4(1.0f);

	for (unsigned int i = 0; i < 4; i++)
	{
		m_lightShader->SetVec3("light[" + std::to_string(i) + "].Position", scene.GetLights()[0]->GetPosition());
		m_lightShader->SetVec3("light[" + std::to_string(i) + "].Color", scene.GetLights()[0]->GetColor());

		model = glm::mat4(1.0f);
		model = glm::translate(model, scene.GetLights()[0]->GetPosition());
	}

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::processInput(GLFWwindow* window, float deltaTime, Scene& scene)
{
	float velocity = 2.5f * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// KEYBOARD W A S D (Camera Movement)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene.GetCameras()[0]->ProcessKeyBoard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene.GetCameras()[0]->ProcessKeyBoard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene.GetCameras()[0]->ProcessKeyBoard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene.GetCameras()[0]->ProcessKeyBoard(RIGHT, deltaTime);

	// KEYBOARD U H J K (Light Movement)
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
		{
			float y = scene.GetLights()[i]->GetPositionY();
			scene.GetLights()[i]->SetPositionY(y += velocity);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
		{
			float y = scene.GetLights()[i]->GetPositionY();
			scene.GetLights()[i]->SetPositionY(y -= velocity);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
		{
			float x = scene.GetLights()[i]->GetPositionX();
			scene.GetLights()[i]->SetPositionX(x -= velocity);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
		{
			float x = scene.GetLights()[i]->GetPositionX();
			scene.GetLights()[i]->SetPositionX(x += velocity);
		}
	}

	// KEYBOARD F N (Light Distance Movement)
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
		{
			float z = scene.GetLights()[i]->GetPositionZ();
			scene.GetLights()[i]->SetPositionZ(z += velocity);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
		{
			float z = scene.GetLights()[i]->GetPositionZ();
			scene.GetLights()[i]->SetPositionZ(z -= velocity);
		}
	}

	// Reset Cam and LightPos
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		scene.GetCameras()[0]->SetPos(glm::vec3(0.0f, 0.0f, 5.0f));

		for (unsigned int i = 0; i < scene.GetLights().size(); ++i)
			scene.GetLights()[i]->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));
	}

	// Set Shadow On/Off
	if (!m_bShadows && (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS))
	{
		m_bShadows = true;
	}
	else if (m_bShadows && (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE))
	{
		m_bShadows = false;
	}
}