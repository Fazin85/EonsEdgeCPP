#include "block_texture.h"
#include "compress.h"
#include "level_renderer.h"
#include "log.h"
#include "settings.h"
#include <lz4.h>
#include <zlib.h>

static float Distance(const glm::vec3& pos1, const glm::vec3& pos2)
{
	return sqrtf((pos1.x - pos2.x) * (pos1.x - pos2.x) + (pos1.y - pos2.y) * (pos1.y - pos2.y) + (pos1.z - pos2.z) * (pos1.z - pos2.z));
}

namespace Eon
{
	LevelRenderer::LevelRenderer()
	{
		level = nullptr;
		exit = false;
		meshing_all_chunks = false;
		meshing_all_meshed_chunks_count = 0;

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
	}

	void LevelRenderer::MeshAllChunks()
	{
		meshing_all_chunks = true;
		meshing_all_meshed_chunks_count = 0;

		for (int x = 0; x < LEVEL_WIDTH_CHUNKS; x++)
		{
			for (int z = 0; z < LEVEL_WIDTH_CHUNKS; z++)
			{
				MeshChunk(ChunkPosition(x, z));
			}
		}
	}

	bool LevelRenderer::MeshingAllChunks() const
	{
		return meshing_all_chunks;
	}

	void LevelRenderer::RemoveMesh(ChunkPosition chunkPosition)
	{
		for (int i = 0; i < chunk_renderers.size(); i++)
		{
			if (chunk_renderers.contains(chunkPosition))
			{
				chunk_renderers.erase(chunkPosition);
			}
		}
	}

	void LevelRenderer::Update(glm::vec3 cameraPosition)
	{
		ChunkPosition position;

		if (meshes_to_setup.try_dequeue(position))
		{
			if (!chunk_renderers.contains(position))
			{
				EON_ERROR("OH NO");
			}
			else
			{
				chunk_renderers[position]->Setup();
			}
		}

		ChunkPosition playerChunkPosition = ChunkPosition((int)cameraPosition.x >> CHUNK_BITSHIFT_AMOUNT, (int)cameraPosition.z >> CHUNK_BITSHIFT_AMOUNT);

		for (int cx = 0; cx < LEVEL_WIDTH_CHUNKS; cx++)
		{
			for (int cz = 0; cz < LEVEL_WIDTH_CHUNKS; cz++)
			{
				ChunkPosition position(cx, cz);

				if (chunk_renderers.contains(ChunkPosition(cx + 1, cz)) && chunk_renderers.contains(ChunkPosition(cx - 1, cz))
					&& chunk_renderers.contains(ChunkPosition(cx, cz + 1)) && chunk_renderers.contains(ChunkPosition(cx, cz - 1))
					&& position != playerChunkPosition)
				{
					level->GetChunk(ChunkPosition(cx, cz))->Compress();
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

		static std::vector<std::pair<LODChunkRenderer*, float>> renderers;
		renderers.clear();

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			float distance = Distance(playerChunkPosition, glm::vec3(chunkPosition.x * CHUNK_WIDTH, 0, chunkPosition.z * CHUNK_WIDTH));
			if (distance > GameSettings.render_distance * CHUNK_WIDTH ||
				!camera.GetFrustum().BoxInFrustum(chunkRenderer->GetAABB()))
			{
				continue;
			}

			renderers.emplace_back(chunkRenderer.get(), distance);
		}

		std::sort(renderers.begin(), renderers.end(), [cameraPosition](std::pair<LODChunkRenderer*, float> first, std::pair<LODChunkRenderer*, float> second) -> bool
			{
				ChunkPosition firstPosition = first.first->GetChunk().Position();
				ChunkPosition secondPosition = second.first->GetChunk().Position();

				return Distance(cameraPosition, glm::vec3(firstPosition.x, 0, firstPosition.z)) > Distance(cameraPosition, glm::vec3(secondPosition.x, 0, secondPosition.z));
			});

		for (const auto& pair : renderers)
		{
			unsigned int lod = GetLod(pair.second);
			ChunkPosition chunkPosition = pair.first->GetChunk().Position();

			chunk_shader->UniformFVec3("chunkPos", glm::vec3(chunkPosition.x * CHUNK_WIDTH, -((int)lod * CHUNK_MAX_LOD), chunkPosition.z * CHUNK_WIDTH));

			pair.first->Render(lod);
		}
	}

	int LevelRenderer::ChunkRendererCount()
	{
		return chunk_renderers.size();
	}

	void LevelRenderer::MeshThread()
	{
		//is this a good idea?
		/*HANDLE thread = GetCurrentThread();
		if (SetThreadPriority(thread, THREAD_PRIORITY_HIGHEST) == 0)
		{
			EON_ERROR("Failed to set thread priority");
		}*/
		//it was not a good idea.

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
		Chunk* chunk = level->GetChunk(inChunkPosition);
		glm::ivec3 chunkPosition(chunk->Position().x * CHUNK_WIDTH, 0, chunk->Position().z * CHUNK_WIDTH);
		sf::Clock timer;
		std::vector<ChunkRenderer*> chunkRenderers;


		ChunkMeshConstructionData opaqueMeshData{};
		ChunkMeshConstructionData transparentMeshData{};
		bool tranparency = false;

		for (unsigned char x = 0; x < CHUNK_WIDTH; x++)
		{
			for (short y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (unsigned char z = 0; z < CHUNK_WIDTH; z++)
				{
					int numOpaqueFaces = 0;
					int numTransparentFaces = 0;
					glm::ivec3 position(x, y, z);
					Block* block = chunk->GetBlock(x, y, z);
					bool blockTransparent = block->Transparent();
					short height = *chunk->GetHeightestBlockY(x, z);

					if (block == nullptr)
					{
						EON_ERROR("Block was nullptr");
						break;
					}

					if (block->type == BlockType::AIR)
					{
						continue;
					}

					if (blockTransparent)
					{
						tranparency = true;
					}

					BlockType type = block->type;

					Directions dir = Directions::Left;
					if (x > 0)
					{
						if (chunk->GetBlock(x - 1, y, z)->Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x - 1, y, z));
						if (sideBlock != nullptr)
						{
							if (sideBlock->Transparent() || blockTransparent)
							{
								AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
								blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
							}
						}
					}

					dir = Directions::Right;
					if (x < CHUNK_WIDTH - 1)
					{
						if (chunk->GetBlock(x + 1, y, z)->Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x + 1, y, z));
						if (sideBlock != nullptr)
						{
							if (sideBlock->Transparent() || blockTransparent)
							{
								AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
								blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
							}
						}
					}

					dir = Directions::Top;
					if (y < CHUNK_HEIGHT - 1)
					{
						if (chunk->GetBlock(x, y + 1, z)->Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
						blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
					}

					dir = Directions::Bottom;
					if (y > 0)
					{
						if (chunk->GetBlock(x, y - 1, z)->Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Front;
					if (z < CHUNK_WIDTH - 1)
					{
						if (chunk->GetBlock(x, y, z + 1)->Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x, y, z + 1));
						if (sideBlock != nullptr)
						{
							if (sideBlock->Transparent() || blockTransparent)
							{
								AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
								blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
							}
						}
					}

					dir = Directions::Back;
					if (z > 0)
					{
						if (chunk->GetBlock(x, y, z - 1)->Transparent() || blockTransparent)
						{
							AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
							blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x, y, z - 1));
						if (sideBlock != nullptr)
						{
							if (sideBlock->Transparent() || blockTransparent)
							{
								AddFace(blockTransparent ? transparentMeshData : opaqueMeshData, position, type, dir, 1);
								blockTransparent ? numTransparentFaces++ : numOpaqueFaces++;
							}
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

		ChunkRenderer* chunkRenderer = new ChunkRenderer(opaqueMeshData);
		if (transparentMeshData.vertexPositions.size() > 0)
		{
			chunkRenderer->SetWaterMesh(new ChunkRenderer(transparentMeshData));
		}
		chunkRenderers.emplace_back(chunkRenderer);


		if (chunk_renderers.contains(chunk->Position()))
		{
			chunk_renderers.erase(chunk->Position());
		}

		chunk_renderers[chunk->Position()] = std::make_unique<LODChunkRenderer>(chunk, chunkRenderers);

		meshes_to_setup.enqueue(chunk->Position());

		auto time = timer.getElapsedTime();
		meshing_all_chunks_time_sum += time.asMilliseconds();

		if (meshing_all_chunks)
		{
			meshing_all_meshed_chunks_count++;

			if (meshing_all_meshed_chunks_count == LEVEL_WIDTH_CHUNKS * LEVEL_WIDTH_CHUNKS)
			{
				meshing_all_chunks = false;
				meshing_all_meshed_chunks_count = 0;

				EON_INFO("Overall chunk meshing time: " + std::to_string(meshing_all_chunks_time_sum));
				EON_INFO("AVG chunk meshing time: " + std::to_string((float)meshing_all_chunks_time_sum / ((float)LEVEL_WIDTH_CHUNKS * (float)LEVEL_WIDTH_CHUNKS)));
				meshing_all_chunks_time_sum = 0;
			}
		}
	}

	void LevelRenderer::AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction, unsigned int lod)
	{
		auto faceData = GetFaceDataFromDirection(direction);

		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int x = (faceData[index++] * lod) + blockPosition.x;
			int y = (faceData[index++] * lod) + blockPosition.y;
			int z = (faceData[index++] * lod) + blockPosition.z;

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
}  // namespace Eon