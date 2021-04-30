#pragma once
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

#include <iostream>

class Shader;
class Scene;
class GBuffer;
class Renderer
{
public:
    Renderer();

	~Renderer();

	void StartRenderer(unsigned int width, unsigned int height);

	void Render(Scene& scene)
	{
		this->ClearBackBuffer();

		this->ShadowPass(scene);
		this->GeometryPass(scene);
		this->LightPass(scene);
	}

	void GeometryPass(Scene& scene);

	void ShadowPass(Scene& scene);

	void LightPass(Scene& scene);

	void processInput(GLFWwindow* window, float deltaTime, Scene& scene);

	void ClearBackBuffer()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

private:
	unsigned int m_winWidth;
	unsigned int m_winHeight;

	unsigned int m_shadowWidth;
	unsigned int m_shadowHeight;

	unsigned int m_quadVAO;
	unsigned int m_quadVBO;

	unsigned int m_depthMapFBO;
	unsigned int m_depthMap;

	unsigned int m_entityNum;
	unsigned int m_lightNum;

	bool m_bShadows;

	Shader* m_geometryShader;
	Shader* m_lightShader;
	Shader* m_shadowShader;

	GBuffer* m_GBuffer;
};