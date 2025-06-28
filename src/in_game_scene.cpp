#include "in_game_scene.h"
#include "basic_terrain_generator.h"
#include "default_chunk_renderer_container_provider.h"
#include "settings.h"
#include "block_entity_test.h"

namespace Eon
{
	std::unique_ptr<PositionTextureColorNormalMesh> CreateCubeMesh(
		float size = 1.0f,
		const glm::vec3& color = glm::vec3(1.0f, 1.0f, 1.0f),
		Texture* texture = nullptr,
		const PoolAllocator<glm::vec3>& vec3_allocator = *PoolAllocators::GetInstance().vec3_allocator,
		const PoolAllocator<glm::vec2>& vec2_allocator = *PoolAllocators::GetInstance().vec2_allocator)
	{
		float half_size = size * 0.5f;

		// Create vertices for a cube (36 vertices - 6 faces * 2 triangles * 3 vertices)
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions(vec3_allocator);
		std::vector<glm::vec2, PoolAllocator<glm::vec2>> texture_coords(vec2_allocator);
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> colors(vec3_allocator);
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> normals(vec3_allocator);

		positions.reserve(36);
		texture_coords.reserve(36);
		colors.reserve(36);
		normals.reserve(36);

		// Front face
		positions.insert(positions.end(), {
			{-half_size, -half_size,  half_size}, // Bottom-left
			{ half_size, -half_size,  half_size}, // Bottom-right
			{ half_size,  half_size,  half_size}, // Top-right
			{ half_size,  half_size,  half_size}, // Top-right
			{-half_size,  half_size,  half_size}, // Top-left
			{-half_size, -half_size,  half_size}  // Bottom-left
			});

		texture_coords.insert(texture_coords.end(), {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}
			});

		for (int i = 0; i < 6; ++i)
		{
			colors.push_back(color);
			normals.push_back({ 0.0f, 0.0f, 1.0f }); // Front normal
		}

		// Back face
		positions.insert(positions.end(), {
			{ half_size, -half_size, -half_size}, // Bottom-left
			{-half_size, -half_size, -half_size}, // Bottom-right
			{-half_size,  half_size, -half_size}, // Top-right
			{-half_size,  half_size, -half_size}, // Top-right
			{ half_size,  half_size, -half_size}, // Top-left
			{ half_size, -half_size, -half_size}  // Bottom-left
			});

		texture_coords.insert(texture_coords.end(), {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}
			});

		for (int i = 0; i < 6; ++i)
		{
			colors.push_back(color);
			normals.push_back({ 0.0f, 0.0f, -1.0f }); // Back normal
		}

		// Left face
		positions.insert(positions.end(), {
			{-half_size, -half_size, -half_size}, // Bottom-left
			{-half_size, -half_size,  half_size}, // Bottom-right
			{-half_size,  half_size,  half_size}, // Top-right
			{-half_size,  half_size,  half_size}, // Top-right
			{-half_size,  half_size, -half_size}, // Top-left
			{-half_size, -half_size, -half_size}  // Bottom-left
			});

		texture_coords.insert(texture_coords.end(), {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}
			});

		for (int i = 0; i < 6; ++i)
		{
			colors.push_back(color);
			normals.push_back({ -1.0f, 0.0f, 0.0f }); // Left normal
		}

		// Right face
		positions.insert(positions.end(), {
			{ half_size, -half_size,  half_size}, // Bottom-left
			{ half_size, -half_size, -half_size}, // Bottom-right
			{ half_size,  half_size, -half_size}, // Top-right
			{ half_size,  half_size, -half_size}, // Top-right
			{ half_size,  half_size,  half_size}, // Top-left
			{ half_size, -half_size,  half_size}  // Bottom-left
			});

		texture_coords.insert(texture_coords.end(), {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}
			});

		for (int i = 0; i < 6; ++i)
		{
			colors.push_back(color);
			normals.push_back({ 1.0f, 0.0f, 0.0f }); // Right normal
		}

		// Top face
		positions.insert(positions.end(), {
			{-half_size,  half_size,  half_size}, // Bottom-left
			{ half_size,  half_size,  half_size}, // Bottom-right
			{ half_size,  half_size, -half_size}, // Top-right
			{ half_size,  half_size, -half_size}, // Top-right
			{-half_size,  half_size, -half_size}, // Top-left
			{-half_size,  half_size,  half_size}  // Bottom-left
			});

		texture_coords.insert(texture_coords.end(), {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}
			});

		for (int i = 0; i < 6; ++i)
		{
			colors.push_back(color);
			normals.push_back({ 0.0f, 1.0f, 0.0f }); // Top normal
		}

		// Bottom face
		positions.insert(positions.end(), {
			{-half_size, -half_size, -half_size}, // Bottom-left
			{ half_size, -half_size, -half_size}, // Bottom-right
			{ half_size, -half_size,  half_size}, // Top-right
			{ half_size, -half_size,  half_size}, // Top-right
			{-half_size, -half_size,  half_size}, // Top-left
			{-half_size, -half_size, -half_size}  // Bottom-left
			});

		texture_coords.insert(texture_coords.end(), {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}
			});

		for (int i = 0; i < 6; ++i)
		{
			colors.push_back(color);
			normals.push_back({ 0.0f, -1.0f, 0.0f }); // Bottom normal
		}

		// Create and return the mesh
		return std::make_unique<PositionTextureColorNormalMesh>(
			std::move(positions),
			std::move(texture_coords),
			std::move(colors),
			std::move(normals),
			texture
		);
	}

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

		sf::Image image;
		image.loadFromFile("content/images/test.png");
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

		auto t = new Texture(image, false);

		cube = CreateCubeMesh(
			256.0f,
			glm::vec3(1.0f),
			t,
			*PoolAllocators::GetInstance().vec3_allocator,
			*PoolAllocators::GetInstance().vec2_allocator);

		cube->Setup();

		ptcn_shader = std::make_unique<Shader>("ptcn.vert", "ptcn.frag");
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
				chunk->AddBlockEntity(std::make_unique<BlockEntityTest>(pos));
				done_thing = true;
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
			EON_INFO(ss.str());
			counter = 0;
		}

		level->Update(ChunkPosition{ static_cast<int>(player->Position().x), static_cast<int>(player->Position().z) }, GameSettings.render_distance + 2);

		player->Update(dt);
	}

	void InGameScene::Render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		level_renderer->Render(player->GetCamera(), player->Position());

		Mesh::RenderMeshes(player->GetCamera(), player->Position(), { cube.get() }, *ptcn_shader);

		sprite->Render(player->GetCamera(), player->Position());

		skybox->Render(player->GetCamera());
	}

	const char* InGameScene::GetName()
	{
		return "InGame";
	}
}
