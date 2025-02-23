#include "Application.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/Scene.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

double deltaTime = 0.0f;
double lastFrameTime = 0.0f;
int frameCounter = 0;
double fpsTimeCounter = glfwGetTime();

Application::Application(const std::string& title, unsigned int width, unsigned int height) :
	m_title(title),
	m_renderer(nullptr),
	m_winWidth(width),
	m_winHeight(height),
	m_window(nullptr)
{
}

Application::~Application()
{
	if (m_renderer != nullptr)
	{
		delete m_renderer;
		m_renderer = nullptr;
	}

	if (m_window != nullptr)
	{
		delete m_window;
		m_window = nullptr;
	}
}

int Application::Run(Scene& scene)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(m_winWidth, m_winHeight, m_title.c_str(), nullptr, nullptr);
	if (m_window == NULL)
	{
		std::cout << "Failed to Create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(m_window);
	glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

	if (gl3wInit())
	{
		fprintf(stderr, "failed to initialize OpenGL\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	scene.Init();
	m_renderer = new Renderer();
	m_renderer->StartRenderer(m_winWidth, m_winHeight);

	while (!glfwWindowShouldClose(m_window))
	{
		++frameCounter;

		const double currentFrameTime = glfwGetTime();
		
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// Show FPS
		if ((currentFrameTime - fpsTimeCounter) >= 1.0) 
		{
			double actualElapsedTime = (currentFrameTime - fpsTimeCounter);

			std::cout << " FPS: " << (frameCounter / actualElapsedTime) << std::endl;

			frameCounter = 0;
			fpsTimeCounter += actualElapsedTime;
		}

		m_renderer->processInput(m_window, deltaTime, scene);
		m_renderer->Render(scene);
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}