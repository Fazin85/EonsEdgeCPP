#include "in_game_scene.h"
#include "basic_terrain_generator.h"
#include "default_chunk_renderer_container_provider.h"
#include "settings.h"
#include "block_entity_test.h"

namespace Eon
{
	InGameScene::InGameScene()
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
	}

	void InGameScene::Update(float dt)
	{
		player->GetCamera().CalculateViewMatrix(player->Position());
		level_renderer->Update(player->GetCamera().GetFrustum(), player->Position());

		static bool done_thing = false;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) && !done_thing)
		{
			if (auto chunk = level->GetChunk(ChunkPosition()); chunk)
			{
				auto pos = glm::ivec3(0, 200, 0);
				chunk->get()->AddBlockEntity(std::make_unique<BlockEntityTest>(pos));
				done_thing = true;
			}
		}

		level->Update(ChunkPosition{ static_cast<int>(player->Position().x), static_cast<int>(player->Position().z) }, GameSettings.render_distance + 2);

		player->Update(dt);
	}

	void InGameScene::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		level_renderer->Render(player->GetCamera(), player->Position());

		sprite->Render(player->GetCamera(), player->Position());

		skybox->Render(player->GetCamera());
	}

	const char* InGameScene::GetName()
	{
		return "InGame";
	}
}
