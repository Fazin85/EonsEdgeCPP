#include "game.h"
#include "window.h"
#include <chrono>
#include <glad/glad.h>
#include <SFML/Window.hpp>
#include <thread>

namespace Eon
{
	int Game::Run()
	{
		exitCode = 0;
		stop = false;
		Log::Init();

		EON_INFO("Starting...");

		Window::Create(1280, 720, 160, "Eon's Edge");

		gladLoadGL();

		Init();

		EON_INFO("Started successfully");

		sf::Clock clock;

		while (!stop)
		{
			sf::Event event;
			while (Window::Get().pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					stop = true;
				}
				else if (event.type == sf::Event::Resized)
				{
					glViewport(0, 0, event.size.width, event.size.height);
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || stop)
			{
				break;
			}

			sf::Time time = clock.restart();
			float dt = time.asSeconds() * 160;

			Update(dt);

			Render();

			Window::Get().display();

			std::this_thread::sleep_for(std::chrono::milliseconds(6));
		}

		EON_INFO("Shutting down...");

		OnExit();

		EON_INFO("Shut down successfully");

		return exitCode;
	}

	void Game::Stop(int exitCode)
	{
		this->exitCode = exitCode;
		stop = true;
	}

	void Game::Init()
	{
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		Window::Get().setMouseCursorVisible(false);

		player = std::make_unique<Player>();
		level = std::make_unique<Level>();

		level->SkyColor() = glm::vec4(0.39f, 0.52f, 0.745f, 1.0f);

		level_renderer = std::make_unique<LevelRenderer>();
		level_renderer->SetLevel(level.get());

		for (int x = 0; x < 32; x++)
		{
			for (int z = 0; z < 32; z++)
			{
				level_renderer->MeshChunk(ChunkPosition(x, z));
			}
		}
	}

	void Game::Update(float dt)
	{
		level_renderer->Update();

		player->Update(dt);
	}

	void Game::Render()
	{
		level_renderer->Render(*player.get());
	}

	void Game::OnExit()
	{

	}
}
