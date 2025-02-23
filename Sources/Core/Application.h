#pragma once
#include <string>
#include <vector>

class Renderer;
class Scene;
struct GLFWwindow;
class Application
{
public:
	Application(const std::string& title, unsigned int width, unsigned int height);
	~Application();

	int Run(Scene& scene);

private:
	std::string m_title;
	GLFWwindow* m_window;

	Renderer* m_renderer;

	unsigned int m_winWidth;
	unsigned int m_winHeight;
};