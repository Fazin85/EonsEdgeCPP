#include "block_texture.h"
#include "compress.h"
#include "level_renderer.h"
#include "log.h"
#include "settings.h"
#include <lz4.h>
#include <zlib.h>

namespace Eon
{
	LevelRenderer::LevelRenderer()
	{
		level = nullptr;
		exit = false;

		for (int i = 0; i < GameSettings.mesh_gen_threads_count; i++)
		{
			mesh_threads.emplace_back(&LevelRenderer::MeshThread, this);
		}

		chunk_shader = std::make_unique<Shader>("Chunk.vert", "Chunk.frag");

		chunk_shader->Bind();
		chunk_shader->UniformFVec3("sunlight_dir", glm::vec3(-0.7f, -1.0f, -0.5f));
		chunk_shader->UniformFVec3("light_color", glm::vec3(1, 1, 1));
		chunk_shader->UniformFloat("ambient_light", 0.15f);
		chunk_shader->UniformMatrix4("model", glm::mat4(1.0f));
		chunk_shader->UniformFloat("fog_near", ((GameSettings.render_distance * CHUNK_WIDTH) / 2) + CHUNK_WIDTH);
		chunk_shader->UniformInt1("textureSampler", 0);

		int fogFar = GameSettings.fog ? GameSettings.render_distance * CHUNK_WIDTH - CHUNK_WIDTH : 100000;
		chunk_shader->UniformFloat("fog_far", fogFar);

		std::vector<std::string> imageNames;
		imageNames.emplace_back("Error.png");
		imageNames.emplace_back("StoneBlock.png");
		imageNames.emplace_back("GrassBlockTop.png");
		imageNames.emplace_back("GrassBlockSide.png");
		imageNames.emplace_back("DirtBlock.png");
		imageNames.emplace_back("WaterBlock.png");
		imageNames.emplace_back("SandBlock.png");
		imageNames.emplace_back("OakLogTop.png");
		imageNames.emplace_back("OakLogSide.png");
		imageNames.emplace_back("LeafBlock.png");
		imageNames.emplace_back("GravelBlock.png");

		chunk_texture = std::make_unique<TextureArray>(imageNames, 16, 16);
	}

	LevelRenderer::~LevelRenderer()
	{
		this->exit = true;

		for (int i = 0; i < mesh_threads.size(); i++)
		{
			mesh_threads[i].join();
		}
	}

	void LevelRenderer::SetLevel(Level* level)
	{
		this->level = level;
	}

	void LevelRenderer::MeshChunk(ChunkPosition chunkPosition)
	{
		chunks_to_mesh.enqueue(chunkPosition);
		chunks_to_mesh_vector.push_back(chunkPosition);
	}

	void LevelRenderer::RemoveMesh(ChunkPosition chunkPosition)
	{
		if (chunk_renderers.contains(chunkPosition))
		{
			chunk_renderers.erase(chunkPosition);
		}
	}

	void LevelRenderer::Update(glm::vec3 cameraPosition)
	{
		std::unique_ptr<AABBChunkRenderer> chunk;
		ChunkPosition position;

		if (meshes_to_setup.try_dequeue(chunk))
		{
			position = chunk->GetChunk().Position();

			if (chunk_renderers.contains(position)) {
				chunk_renderers.erase(position);
			}

			chunk_renderers[position] = std::move(chunk);
			chunk_renderers[position]->Setup();

			chunks_to_mesh_vector.erase(std::remove(chunks_to_mesh_vector.begin(), chunks_to_mesh_vector.end(), position), chunks_to_mesh_vector.end());
			MarkCanUnloadForMeshing(position, true);
		}

		glm::vec3 playerChunkPosition = glm::vec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

		for (int cx = playerChunkPosition.x - (CHUNK_WIDTH * GameSettings.render_distance); cx <= playerChunkPosition.x + (CHUNK_WIDTH * GameSettings.render_distance); cx += CHUNK_WIDTH) {
			for (int cz = playerChunkPosition.z - (CHUNK_WIDTH * GameSettings.render_distance); cz <= playerChunkPosition.z + (CHUNK_WIDTH * GameSettings.render_distance); cz += CHUNK_WIDTH) {
				ChunkPosition currentChunkPosition(cx, cz);

				if (CanChunkBeMeshed(currentChunkPosition)) {
					MarkCanUnloadForMeshing(currentChunkPosition, false);
					MeshChunk(currentChunkPosition);
				}
			}
		}
	}

	void LevelRenderer::Render(Camera& camera, glm::vec3 cameraPosition)
	{
		if (level == nullptr)
		{
			return;
		}

		glCullFace(GL_BACK);
		glClearColor(level->SkyColor().r, level->SkyColor().g, level->SkyColor().b, level->SkyColor().a);
		chunk_texture->Bind();

		chunk_shader->Bind();
		chunk_shader->UniformFVec4("fog_color", level->SkyColor());
		camera.CalculateViewMatrix(glm::vec3(0, 0, 0));
		chunk_shader->UniformMatrix4("view", *camera.ViewMatrix());
		chunk_shader->UniformMatrix4("projection", *camera.ProjectionMatrix());
		chunk_shader->UniformFVec3("camPos", cameraPosition);

		camera.CalculateViewMatrix(cameraPosition);

		glm::vec3 playerChunkPosition = glm::vec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

		static std::vector<std::pair<AABBChunkRenderer*, float>> renderers;
		renderers.clear();

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			float distance = glm::distance(playerChunkPosition, glm::vec3(chunkPosition.x, 0, chunkPosition.z));

			if (distance > GameSettings.render_distance * CHUNK_WIDTH ||
				!camera.GetFrustum().BoxInFrustum(chunkRenderer->GetAABB()))
			{
				continue;
			}

			renderers.emplace_back(chunkRenderer.get(), distance);
		}

		std::sort(renderers.begin(), renderers.end(), [cameraPosition](std::pair<AABBChunkRenderer*, float> first, std::pair<AABBChunkRenderer*, float> second) -> bool
			{
				ChunkPosition firstPosition = first.first->GetChunk().Position();
				ChunkPosition secondPosition = second.first->GetChunk().Position();

				return glm::distance(cameraPosition, glm::vec3(firstPosition.x, 0, firstPosition.z)) > glm::distance(cameraPosition, glm::vec3(secondPosition.x, 0, secondPosition.z));
			});

		for (const auto& pair : renderers)
		{
			unsigned int lod = GetLod(pair.second);
			ChunkPosition chunkPosition = pair.first->GetChunk().Position();

			chunk_shader->UniformFVec3("chunkPos", glm::vec3(chunkPosition.x, 0.0f, chunkPosition.z));

			pair.first->Render(lod);
		}
	}

	int LevelRenderer::ChunkRendererCount()
	{
		return chunk_renderers.size();
	}

	bool LevelRenderer::IsChunkBeingMeshed(ChunkPosition position)
	{
		return std::find(chunks_to_mesh_vector.begin(), chunks_to_mesh_vector.end(), position) != chunks_to_mesh_vector.end();
	}

	void LevelRenderer::OnChunkUnloaded(Chunk& chunk)
	{
		RemoveMesh(chunk.Position());
	}

	void LevelRenderer::MeshThread()
	{
		while (!exit)
		{
			ChunkPosition chunkPosition;
			if (chunks_to_mesh.try_dequeue(chunkPosition))
			{
				BuildChunkMesh(chunkPosition);
			}
		}
	}

	void LevelRenderer::BuildChunkMesh(ChunkPosition inChunkPosition)
	{
		auto chunk = level->GetChunk(inChunkPosition);

		if (!chunk.has_value()) {
			std::stringstream ss{};
			ss << "Failed to get chunk at " << inChunkPosition.x << "," << inChunkPosition.z << "\n";
			EON_WARN(ss.str());
			return;
		}

		glm::ivec3 chunkPosition(chunk->get().Position().x, 0, chunk->get().Position().z);
		sf::Clock timer;

		ChunkMeshConstructionData opaqueMeshData{};
		ChunkMeshConstructionData transparentMeshData{};
		bool tranparency = false;

		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_WIDTH; z++)
				{
					int numOpaqueFaces = 0;
					int numTransparentFaces = 0;
					glm::ivec3 position(x, y, z);
					Block block = chunk->get().GetBlock(x, y, z);
					bool blockTransparent = block.Transparent();
					short height = chunk->get().GetHeightestBlockY(x, z);

					if (block.type == BlockType::AIR)
					{
						continue;
					}

					if (blockTransparent)
					{
						tranparency = true;
					}

					BlockType type = block.type;

					Directions dir = Directions::Left;
					if (x > 0)
					{
						if (chunk->get().GetBlock(x - 1, y, z).Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x - 1, y, z));
						if (sideBlock.Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Right;
					if (x < CHUNK_WIDTH - 1)
					{
						if (chunk->get().GetBlock(x + 1, y, z).Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x + 1, y, z));
						if (sideBlock.Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Top;
					if (y < CHUNK_HEIGHT - 1)
					{
						if (chunk->get().GetBlock(x, y + 1, z).Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
						blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
					}

					dir = Directions::Bottom;
					if (y > 0)
					{
						if (chunk->get().GetBlock(x, y - 1, z).Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Front;
					if (z < CHUNK_WIDTH - 1)
					{
						if (chunk->get().GetBlock(x, y, z + 1).Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x, y, z + 1));
						if (sideBlock.Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Back;
					if (z > 0)
					{
						if (chunk->get().GetBlock(x, y, z - 1).Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x, y, z - 1));

						if (sideBlock.Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					if (opaqueMeshData.vertexPositions.size() > 0)
					{
						AddIndices(opaqueMeshData, numOpaqueFaces);
					}

					if (transparentMeshData.vertexPositions.size() > 0)
					{
						AddIndices(transparentMeshData, numTransparentFaces);
					}
				}
			}
		}

		std::unique_ptr<ChunkRenderer> chunkRenderer = std::make_unique<ChunkRenderer>(opaqueMeshData);
		if (transparentMeshData.vertexPositions.size() > 0)
		{
			chunkRenderer->SetWaterMesh(std::make_unique<ChunkRenderer>(transparentMeshData));
		}

		meshes_to_setup.enqueue(std::move(std::make_unique<AABBChunkRenderer>(chunk->get(), std::move(chunkRenderer))));
	}

	void LevelRenderer::AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction)
	{
		auto faceData = GetFaceDataFromDirection(direction);

		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int x = faceData[index++] + blockPosition.x;
			int y = faceData[index++] + blockPosition.y;
			int z = faceData[index++] + blockPosition.z;

			meshData.vertexPositions.emplace_back(x, y, z);
			meshData.directions.push_back(static_cast<unsigned char>(direction));
			meshData.light.push_back(15);
			meshData.uvs.push_back({ i, GetTextureId(blockType, direction) });
		}
	}

	std::array<unsigned char, 12> LevelRenderer::GetFaceDataFromDirection(Directions dir)
	{
		switch (dir)
		{
		case Directions::Front:
			return std::array<unsigned char, 12>({ 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1 });
		case Directions::Back:
			return std::array<unsigned char, 12>({ 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0 });
		case Directions::Left:
			return std::array<unsigned char, 12>({ 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0 });
		case Directions::Right:
			return std::array<unsigned char, 12>({ 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1 });
		case Directions::Top:
			return std::array<unsigned char, 12>({ 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0 });
		case Directions::Bottom:
			return std::array<unsigned char, 12>({ 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 });
		}

		return {};
	}

	void LevelRenderer::AddIndices(ChunkMeshConstructionData& meshData, int count)
	{
		for (int i = 0; i < count; i++)
		{
			meshData.indices.push_back(meshData.index_count);
			meshData.indices.push_back(meshData.index_count + 1);
			meshData.indices.push_back(meshData.index_count + 2);
			meshData.indices.push_back(meshData.index_count + 2);
			meshData.indices.push_back(meshData.index_count + 3);
			meshData.indices.push_back(meshData.index_count);

			meshData.index_count += 4;
		}
	}

	unsigned int LevelRenderer::GetLod(float distance)
	{
		if (!GameSettings.lod)
		{
			return 0;
		}

		int width = GameSettings.percent_lod ? CHUNK_WIDTH : CHUNK_WIDTH / 2;
		unsigned int lod = 0;

		if (distance > (width * GameSettings.render_distance) / 4.0f)
		{
			lod++;
		}
		if (distance > (width * GameSettings.render_distance) / 2.0f)
		{
			lod++;
		}
		if (distance > (width * GameSettings.render_distance) / 1.5f)
		{
			lod++;
		}

		return lod;
	}

	BlockFaceTexture LevelRenderer::GetTextureId(BlockType blockType, Directions faceDirection)
	{
		switch (blockType)
		{
		case Eon::BlockType::AIR:
			return BlockFaceTexture::ERR;
		case Eon::BlockType::STONE:
			return BlockFaceTexture::STONE;
		case Eon::BlockType::GRASS:
			switch (faceDirection)
			{
			case Eon::Directions::Front:
				return BlockFaceTexture::GRASSIDE;
			case Eon::Directions::Back:
				return BlockFaceTexture::GRASSIDE;
			case Eon::Directions::Left:
				return BlockFaceTexture::GRASSIDE;
			case Eon::Directions::Right:
				return BlockFaceTexture::GRASSIDE;
			case Eon::Directions::Top:
				return BlockFaceTexture::GRASSTOP;
			case Eon::Directions::Bottom:
				return BlockFaceTexture::DIRT;
			}
			break;
		case Eon::BlockType::DIRT:
			return BlockFaceTexture::DIRT;
		case Eon::BlockType::WATER:
			return BlockFaceTexture::WATER;
		case Eon::BlockType::SAND:
			return BlockFaceTexture::SAND;
		case Eon::BlockType::OAKLOG:
			switch (faceDirection)
			{
			case Eon::Directions::Front:
				return BlockFaceTexture::OAKLOGSIDE;
				break;
			case Eon::Directions::Back:
				return BlockFaceTexture::OAKLOGSIDE;

			case Eon::Directions::Left:
				return BlockFaceTexture::OAKLOGSIDE;

			case Eon::Directions::Right:
				return BlockFaceTexture::OAKLOGSIDE;

			case Eon::Directions::Top:
				return BlockFaceTexture::OAKLOGTOP;

			case Eon::Directions::Bottom:
				return BlockFaceTexture::OAKLOGTOP;
			}
			break;
		case Eon::BlockType::LEAF:
			return Eon::BlockFaceTexture::LEAF;
		case Eon::BlockType::GRAVEL:
			return Eon::BlockFaceTexture::GRAVEL;
		}

		return Eon::BlockFaceTexture::ERR;
	}

	bool LevelRenderer::CanChunkBeMeshed(ChunkPosition position)
	{
		bool flag = !chunk_renderers.contains(position) && !IsChunkBeingMeshed(position);

		bool chunkExists0 = level->ChunkExistsAt(position);
		bool chunkExists1 = level->ChunkExistsAt(position.Offset(CHUNK_WIDTH, 0));
		bool chunkExists2 = level->ChunkExistsAt(position.Offset(0, CHUNK_WIDTH));
		bool chunkExists3 = level->ChunkExistsAt(position.Offset(-CHUNK_WIDTH, 0));
		bool chunkExists4 = level->ChunkExistsAt(position.Offset(0, -CHUNK_WIDTH));
		bool chunkExists5 = level->ChunkExistsAt(position.Offset(CHUNK_WIDTH, CHUNK_WIDTH));
		bool chunkExists6 = level->ChunkExistsAt(position.Offset(-CHUNK_WIDTH, CHUNK_WIDTH));
		bool chunkExists7 = level->ChunkExistsAt(position.Offset(CHUNK_WIDTH, -CHUNK_WIDTH));
		bool chunkExists8 = level->ChunkExistsAt(position.Offset(-CHUNK_WIDTH, -CHUNK_WIDTH));

		return flag && chunkExists0 && chunkExists1 && chunkExists2 && chunkExists3 && chunkExists4 && chunkExists5 && chunkExists6 && chunkExists7 && chunkExists8;
	}

	void LevelRenderer::MarkCanUnloadForMeshing(ChunkPosition position, bool canUnload)
	{
		std::vector<ChunkPosition> positions = {
			position,
			position.Offset(CHUNK_WIDTH, 0),
			position.Offset(0, CHUNK_WIDTH),
			position.Offset(-CHUNK_WIDTH, 0),
			position.Offset(0, -CHUNK_WIDTH),
			position.Offset(CHUNK_WIDTH, CHUNK_WIDTH),
			position.Offset(-CHUNK_WIDTH, CHUNK_WIDTH),
			position.Offset(CHUNK_WIDTH, -CHUNK_WIDTH),
			position.Offset(-CHUNK_WIDTH, -CHUNK_WIDTH)
		};

		for (ChunkPosition& chunkPosition : positions) {
			auto chunk = level->GetChunk(chunkPosition);

			if (chunk.has_value()) {
				chunk->get().SetCanUnload(canUnload);
			}
			else {
				std::stringstream ss;
				ss << "Failed to get chunk at: " << chunkPosition.x << "," << chunkPosition.z << "\n";
				EON_CRITICAL(ss.str());
			}
		}
	}
}  // namespace Eon