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
#include <iostream>

namespace Eon
{
	void APIENTRY openglCallbackFunction(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParam)
	{
		std::string sourceStr = [source]()
			{
				switch (source)
				{
				case GL_DEBUG_SOURCE_API: return "API";
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
				case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
				case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
				case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
				case GL_DEBUG_SOURCE_OTHER: return "OTHER";
				default: return "UNKNOWN";
				}
			}();

		std::string typeStr = [type]()
			{
				switch (type)
				{
				case GL_DEBUG_TYPE_ERROR: return "ERROR";
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED";
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED";
				case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
				case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
				case GL_DEBUG_TYPE_OTHER: return "OTHER";
				default: return "UNKNOWN";
				}
			}();

		std::cout << "OpenGL [" << typeStr << "] " << sourceStr << ": " << message << std::endl;

		if (type == GL_DEBUG_TYPE_ERROR)
		{
			__debugbreak(); // Break on errors
		}
	}

	int Game::Run()
	{
		exit_code = 0;
		stop = false;
		Log::Init();

		EON_INFO("Starting...");

		Window::Create(1920, 1080, GameSettings.max_fps, "Eon's Edge", false);
		gladLoadGL();

#ifdef _DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION,
			0, nullptr, GL_FALSE);
#endif

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
