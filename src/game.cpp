#include "fps.h"
#include "game.h"
#include "settings.h"
#include "window.h"
#include <chrono>
#include <SFML/Window.hpp>
#include <imgui.h>
#include "scene_manager.h"
#include "log.h"
#include "in_game_scene.h"
#include "gl_error_check.h"

namespace Eon
{
	int Game::Run()
	{
		exit_code = 0;
		stop = false;
		Log::Init();

		EON_INFO("Starting...");

		Window::Create(1920, 1080, GameSettings.max_fps, "Eon's Edge", false);
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

			sf::Time time = clock.restart();
			float dt = time.asSeconds();

			Update(dt);

			if (scene_manager->SceneChangedThisUpdate())
			{
				scene_manager->SetSceneChangedThisUpdate(false);
				continue;
			}

			Render();

			Window::Get().display();
		}

		EON_INFO("Shutting down...");

		OnExit();

		EON_INFO("Shut down successfully");

		return exit_code;
	}

	void Game::Stop(int exitCode)
	{
		exit_code = exitCode;
		stop = true;
	}

	void Game::Init()
	{
		scene_manager = std::make_unique<SceneManager>();
		scene_manager->LoadScene(std::make_shared<InGameScene>());

		text_renderer = std::make_unique<TextRenderer>("arial.ttf");
	}

	void Game::Update(float dt)
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
				GL_CHECK(glViewport(0, 0, event.size.width, event.size.height));
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			Stop(0);
		}

		scene_manager->GetCurrentScene()->Update(dt);
	}

	void Game::Render()
	{
		scene_manager->GetCurrentScene()->Render();
		GL_CHECK_ERRORS();


		text_renderer->Begin();
		text_renderer->RenderText("Hello World!", 120, 120, 1, { 1, 1, 1 });
		text_renderer->End();
	}

	void Game::OnExit()
	{
	}
}
