#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "log.h"
#include "num.h"
#include "window.h"
#include "level_renderer.h"

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 512
// levels have 32 chunks in x and z dimension
// levels are 512 blocks in all dimensions
#define LEVEL_CHUNKS_SIZE 32

int main()
{
	Eon::Log::Init();

	EON_INFO("starting...");

	Eon::Window window(640, 480, "Eon's Edge");

	Eon::Camera cam(70, 8, window);

	Eon::LevelRenderer* levelRenderer = new Eon::LevelRenderer();

	EON_INFO("started successfully");

	float lastTime = glfwGetTime();
	float dt = 0;

	while (!window.ShouldClose())
	{
		float currentTime = glfwGetTime();

		dt = currentTime - lastTime;

		levelRenderer->Update();

		cam.Update(1);

		glClear(GL_COLOR_BUFFER_BIT);

		lastTime = currentTime;

		window.SwapBuffers();

		window.PollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(6));
	}

	delete levelRenderer;

	return 0;
}