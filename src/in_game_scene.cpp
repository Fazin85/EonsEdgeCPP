#include "in_game_scene.h"
#include "basic_terrain_generator.h"
#include "default_chunk_renderer_container_provider.h"
#include "settings.h"
#include "block_entity_test.h"
#include "asset_manager.h"
#include "renderer/command/mesh_render_command.h"
#include "default_block_texture_provider.h"
#include <glm/gtc/matrix_transform.hpp>
#include "gui/gui_panel_colored.h"
#include "gui/scaled_resolution.h"
#include "game_time.h"
#include <variant>

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

		DefaultBlockTextureProvider* textureProvider = new DefaultBlockTextureProvider();
		level_renderer = std::make_unique<LevelRenderer>(*level, std::make_unique<DefaultChunkRendererContainerProvider>(*level, *textureProvider));

		level->AddChunkUnloadedEventListener(*level_renderer);

		sf::Image image;
		image.loadFromFile("content/images/test.png");

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

		render_pipeline = std::make_unique<DefaultRenderPipeline>();
		command_pool = std::make_unique<RenderCommandPool>();
	}

	void InGameScene::Update(float dt)
	{
		player->GetCamera().CalculateViewMatrix(player->Position());
		level_renderer->Update(player->GetCamera().GetFrustum(), player->Position());

		static bool doneThing = false;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::G) && !doneThing)
		{
			if (auto chunk = level->GetChunk(ChunkPosition()); chunk)
			{
				auto pos = glm::ivec3(0, 200, 0);
				chunk->AddBlockEntity(std::make_unique<BlockEntityTest>(pos));
				doneThing = true;
			}
		}

		static int counter = 0;
		counter++;
		if (counter == 100)
		{
			std::stringstream ss;
			ss << "Chunk data blocks allocated: " << ChunkSection::chunk_data_pool.GetAllocatedBlocks() << "\n";
			ss << "ivec3 pooled values: " << PoolAllocators::GetInstance().ivec3_allocator->PooledCount() << "\n";
			ss << "ivec2 pooled values: " << PoolAllocators::GetInstance().ivec2_allocator->PooledCount() << "\n";
			ss << "u32 pooled values: " << PoolAllocators::GetInstance().u32_allocator->PooledCount() << "\n";
			ss << "u8 pooled values: " << PoolAllocators::GetInstance().u8_allocator->PooledCount() << "\n";
			ss << "texture binds: " << render_pipeline->GetRenderStats().texture_binds << "\n";
			ss << "shader binds: " << render_pipeline->GetRenderStats().shader_binds << "\n";
			EON_INFO(ss.str());
			counter = 0;
		}

		level->Update(ChunkPosition{ static_cast<int>(player->Position().x), static_cast<int>(player->Position().z) }, GameSettings.render_distance + 2);

		player->Update(dt);
	}

	void InGameScene::Render()
	{
		/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		skybox->Render(player->GetCamera());
		glClear(GL_DEPTH_BUFFER_BIT);*/

		render_pipeline->BeginFrame();
		render_pipeline->Clear();
		player->GetCamera().CalculateViewMatrix(glm::vec3(0.0f));
		
		render_pipeline->SetGlobalUniform("view", player->GetCamera().ViewMatrix());
		render_pipeline->SetGlobalUniform("projection", player->GetCamera().ProjectionMatrix());
		render_pipeline->SetGlobalUniform("invView", glm::inverse(player->GetCamera().ViewMatrix()));
		render_pipeline->SetGlobalUniform("invProjection", glm::inverse(player->GetCamera().ProjectionMatrix()));

		render_pipeline->SetGlobalUniform("lightDirection", glm::vec3(0.2f, -0.9f, 0.1f));
		render_pipeline->SetGlobalUniform("lightColor", glm::vec3(1.0f, 0.9f, 0.7f));

		render_pipeline->SetGlobalUniform("viewPos", glm::vec3(0.0f));
		render_pipeline->SetGlobalUniform("objectColor", glm::vec3(1.0f, 1.0f, 1.0f));
		render_pipeline->SetGlobalUniform("shininess", 16.0f);
		render_pipeline->SetGlobalUniform("blockSampler", 1);
		render_pipeline->SetGlobalUniform("elapsedTime", static_cast<float>(GameTime::GetElapsedTime()));

		render_pipeline->SetGlobalUniform("nearPlane", player->GetCamera().ClippingPlanes().x);
		render_pipeline->SetGlobalUniform("farPlane", player->GetCamera().ClippingPlanes().y);

		level_renderer->Render(*render_pipeline, *command_pool, player->GetCamera(), player->Position());

		render_pipeline->EndFrame();
		render_pipeline->Display();

		/*render_pipeline->BeginFrame();
		glClear(GL_DEPTH_BUFFER_BIT);

		int width = Window::Get().getSize().x;
		int height = Window::Get().getSize().y;
		ScaledResolution resolution(width, height);

		render_pipeline->SetGlobalUniform("view", glm::identity<glm::mat4>());
		render_pipeline->SetGlobalUniform("projection", resolution.CreateProjectionMatrix());

		GuiPanelColored panel(0, 0, 128, 64, 1.0f, 0.0f, 0.0f);
		panel.Render(*render_pipeline, *command_pool, resolution);

		render_pipeline->EndFrame();*/
		command_pool->Reset();
	}

	std::string InGameScene::GetName() const
	{
		return "InGame";
	}
}
