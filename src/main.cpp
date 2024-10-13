#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "log.h"
#include "num.h"
#include "window.h"
#include "level_renderer.h"
#include "player.h"

int main()
{
	Eon::Log::Init();

	EON_INFO("starting...");

	Eon::Window::Create(1280, 720, "Eon's Edge");
	
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	auto player = Eon::Player();
	auto level = std::make_unique<Eon::Level>();

	level->SkyColor() = glm::vec4(0.39f, 0.52f, 0.745f, 1.0f);

	auto levelRenderer = std::make_unique<Eon::LevelRenderer>();
	levelRenderer->SetLevel(level.get());
	levelRenderer->MeshChunk(Eon::ChunkPosition(0, 0));

	EON_INFO("started successfully");

	float lastTime = glfwGetTime();
	float dt = 0;

	while (!Eon::Window::ShouldClose())
	{
		float currentTime = glfwGetTime();

		dt = currentTime - lastTime;

		levelRenderer->Update();

		player.Update(dt);

		glClear(GL_COLOR_BUFFER_BIT);

		lastTime = currentTime;

		levelRenderer->Render(player);

		Eon::Window::SwapBuffers();

		Eon::Window::PollEvents();

		std::this_thread::sleep_for(std::chrono::milliseconds(6));
	}

	return 0;
}