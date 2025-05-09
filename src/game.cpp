#include "fps.h"
#include "game.h"
#include "settings.h"
#include "window.h"
#include <chrono>
#include <SFML/Window.hpp>
#include <imgui.h>
#include "basic_terrain_generator.h"
#include "default_chunk_renderer_container_provider.h"

namespace Eon
{
	int Game::Run()
	{
		exitCode = 0;
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

		level = std::make_unique<Level>(std::make_unique<BasicTerrainGenerator>());
		player = std::make_unique<Player>(level.get());

		level->SkyColor() = glm::vec4(153.0f / 255.0f, 204.0f / 255.0f, 1.0f, 1.0f);

		level_renderer = std::make_unique<LevelRenderer>(*level, std::make_unique<DefaultChunkRendererContainerProvider>(*level));

		level->AddChunkUnloadedEventListener(*level_renderer);

		Image image("Test.png");
		sprite = std::make_unique<BillboardSprite>(image);

		std::array<std::string, 6> facesCubemap =
		{
			"skybox/front.png",
			"skybox/left.png",
			"skybox/top.png",
			"skybox/bottom.png",
			"skybox/back.png",
			"skybox/right.png"
		};

		skybox = std::make_unique<Skybox>(facesCubemap);

		text_renderer = std::make_unique<TextRenderer>("arial.ttf");

		imgui_manager = std::make_unique<ImGuiManager>(Window::Get());
		imgui_manager->Init();
		imgui_manager->SetRenderFunction([this]()
			{
				ImGui::SetNextWindowBgAlpha(0.0f); // Completely transparent background
				ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_Always);
				ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Always);

				ImGuiWindowFlags windowFlags =
					ImGuiWindowFlags_NoDecoration |
					ImGuiWindowFlags_NoTitleBar |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoBackground;

				// Begin the transparent window
				if (ImGui::Begin("##TransparentUI", nullptr, windowFlags))
				{
					// Style the button how you want
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.8f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.9f, 1.0f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.7f, 1.0f));

					// Center the button
					float windowWidth = ImGui::GetWindowSize().x;
					float textWidth = ImGui::CalcTextSize("Play").x;
					float buttonWidth = textWidth + ImGui::GetStyle().FramePadding.x * 2.0f;
					ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);

					// Create the button
					if (ImGui::Button("Play", ImVec2(200, 50)))
					{
						//actionTriggered = true;
					}

					ImGui::PopStyleColor(3);
				}
				ImGui::End();
			});
	}

	void Game::Update(float dt)
	{
		sf::Event event;
		while (Window::Get().pollEvent(event))
		{
			imgui_manager->ProcessEvent(event);

			if (event.type == sf::Event::Closed)
			{
				stop = true;
			}
			else if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
		{
			Stop(0);
		}
		
		player->GetCamera().CalculateViewMatrix(player->Position());
		level_renderer->Update(player->GetCamera().GetFrustum(), player->Position());

		level->Update(ChunkPosition{ static_cast<int>(player->Position().x), static_cast<int>(player->Position().z) }, GameSettings.render_distance + 2);

		player->Update(dt);

		imgui_manager->Update(dt);
	}

	void Game::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		level_renderer->Render(player->GetCamera(), player->Position());

		sprite->Render(player->GetCamera(), player->Position());

		skybox->Render(player->GetCamera());

		text_renderer->Begin();
		text_renderer->RenderText("Hello World!", 120, 120, 1, { 1, 1, 1 });
		text_renderer->End();

		imgui_manager->Render();
	}

	void Game::OnExit()
	{
	}
}
