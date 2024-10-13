#include <glad/glad.h>
#include <SFML/Graphics.hpp>

#include "camera.h"
#include "log.h"
#include "num.h"
#include "level_renderer.h"
#include "player.h"
#include "window.h"

int main()
{
	Eon::Log::Init();

	EON_INFO("starting...");

	Eon::Window::Create(1280, 720, 160, "Eon's Edge");

	gladLoadGL();

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

	sf::Clock clock;

	while (true)
	{
		sf::Event event;
		bool b = false;
		while (Eon::Window::GetWindow().pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				b = true;
			}
			else if (event.type == sf::Event::Resized)
			{
				glViewport(0, 0, event.size.width, event.size.height);
			}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || b)
		{
			break;
		}

		sf::Time time = clock.restart();
		float dt = time.asSeconds() * 160;

		levelRenderer->Update();

		player.Update(dt);

		levelRenderer->Render(player);

		Eon::Window::GetWindow().display();

		std::this_thread::sleep_for(std::chrono::milliseconds(6));
	}

	return 0;
}