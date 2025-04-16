#include "fps.h"
#include "game.h"
#include "settings.h"
#include "window.h"
#include <chrono>
#include <SFML/Window.hpp>
#include "basic_terrain_generator.h"
#include "default_aabb_chunk_renderer_provider.h"

namespace Eon
{
	int Game::Run()
	{
		exitCode = 0;
		stop = false;
		Log::Init();

		EON_INFO("Starting...");

		Window::Create(3440, 1440, GameSettings.max_fps, "Eon's Edge", false);
		gladLoadGL();

		Init();

		EON_INFO("Started successfully");

		sf::Clock clock;
		FPS fps;
		int fpsCounter = 0;

		while (!stop)
		{
			fps.Update();
			fpsCounter++;

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
			float dt = time.asSeconds();

			Update(dt);

			Render();

			Window::Get().display();

			if (fpsCounter % GameSettings.max_fps == 0)
			{
				EON_INFO(fps.Get());
				fpsCounter = 0;

				std::stringstream ss{};

				ss << "player pos " << player->Position().x << "," << player->Position().z << "\n";
				EON_INFO(ss.str());
			}
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

		Window::Get().setMouseCursorVisible(false);

		BasicTerrainGenerator* btg = new BasicTerrainGenerator();

		level = std::make_unique<Level>(*btg);
		player = std::make_unique<Player>(level.get());

		level->SkyColor() = glm::vec4(153.0f / 255.0f, 204.0f / 255.0f, 1.0f, 1.0f);

		auto chunkRendererProvider = new DefaultAABBChunkRendererProvider(*level);

		level_renderer = std::make_unique<LevelRenderer>(*chunkRendererProvider);
		level_renderer->SetLevel(level.get());

		level->AddChunkUnloadedEventListener(*level_renderer);

		Image image("Test.png");
		sprite = std::make_unique<BillboardSprite>(image);

		std::string facesCubemap[6] =
		{
			"skybox/front.png",
			"skybox/left.png",
			"skybox/top.png",
			"skybox/bottom.png",
			"skybox/back.png",
			"skybox/right.png"
		};

		skybox = std::make_unique<Skybox>(facesCubemap);

		Image water("WaterBlock.png");
		Image stone("StoneBlock.png");
	}

	void Game::Update(float dt)
	{
		player->GetCamera().CalculateViewMatrix(player->Position());
		level_renderer->Update(player->GetCamera().GetFrustum(), player->Position());

		level->Update(ChunkPosition(player->Position().x, player->Position().z), GameSettings.render_distance + 2);

		player->Update(dt);
	}

	void Game::Render()
	{
		glm::vec4 skyColor = level->SkyColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		level_renderer->Render(player->GetCamera(), player->Position());

		sprite->Render(player->GetCamera(), player->Position());

		skybox->Render(player->GetCamera(), player->Position());
	}

	void Game::OnExit()
	{
	}
}
