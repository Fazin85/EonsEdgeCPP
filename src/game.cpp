#include "game.h"
#include "window.h"
#include <chrono>
#include <SFML/Window.hpp>
#include <thread>
#include <chrono>
#include "fps.h"

namespace Eon
{
	int Game::Run()
	{
		exitCode = 0;
		stop = false;
		Log::Init();

		EON_INFO("Starting...");

		Window::Create(3440, 1440, 0, "Eon's Edge", false);
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

			if (fpsCounter % 160 == 0)
			{
				EON_INFO(fps.Get());
				fpsCounter = 0;
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

		level = std::make_unique<Level>();
		player = std::make_unique<Player>(level.get());

		level->SkyColor() = glm::vec4(0.39f, 0.52f, 0.745f, 1.0f);

		level_renderer = std::make_unique<LevelRenderer>();
		level_renderer->SetLevel(level.get());

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

		water_planes.reserve(100);

		for (int x = 0; x < 10; x++)
		{
			for (int z = 0; z < 10; z++)
			{
				auto& p = water_planes.emplace_back(std::make_unique<PlaneMesh>(glm::vec3(9216 - 2048 * x, 9216 - 2048 * z, -25.0f), glm::vec2(1024, 1024), water));
				p->Rotate(0, -90);
			}
		}

		bedrock_plane = std::make_unique<PlaneMesh>(glm::vec3(1, 1, -64), glm::vec2(10240, 10240), stone);
		bedrock_plane->Rotate(0, -90);

		level_renderer->MeshAllChunks();
	}

	int idx = 0;
	void Game::Update(float dt)
	{
		level_renderer->Update();

		/*if (idx < LEVEL_WIDTH_CHUNKS * LEVEL_WIDTH_CHUNKS && !level_renderer->MeshingAllChunks())
		{
			auto start = std::chrono::high_resolution_clock::now();

			level->GetChunkFromIndex(idx)->Compress();
			level->GetChunkFromIndex(idx)->Decompress();
			idx++;

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(end - start);
			EON_INFO("time:");
			EON_INFO(duration.count());
		}*/

		player->Update(dt);
	}

	void Game::Render()
	{
		glm::vec4 skyColor = level->SkyColor();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		level_renderer->Render(player->GetCamera(), player->Position());

		sprite->Render(player->GetCamera(), player->Position());

		skybox->Render(player->GetCamera(), player->Position());

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		bedrock_plane->Render(player->GetCamera(), player->Position());

		for (auto& waterPlane : water_planes)
		{
			waterPlane->Render(player->GetCamera(), player->Position());
		}
	}

	void Game::OnExit()
	{

	}
}
