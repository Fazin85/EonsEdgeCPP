#include "level_renderer.h"
#include "log.h"
#include "block_texture.h"

namespace Eon
{
	LevelRenderer::LevelRenderer()
	{
		level = nullptr;
		exit = false;

		mesh_thread = std::thread(&LevelRenderer::MeshThread, this);

		chunk_shader = std::make_unique<Shader>("Chunk.vert", "Chunk.frag");

		chunk_shader->Bind();
		chunk_shader->UniformFVec3("sunlight_dir", glm::vec3(-0.7f, -1.0f, -0.5f));
		chunk_shader->UniformFVec3("light_color", glm::vec3(1, 1, 1));
		chunk_shader->UniformFloat("ambient_light", 0.15f);
		chunk_shader->UniformMatrix4("model", glm::mat4(1.0f));
		chunk_shader->UniformFloat("fog_near", ((12 * 16) / 2) - 16);
		chunk_shader->UniformFloat("fog_far", 100000);

		Image image("BlockAtlas.png");
		chunk_texture = std::make_unique<Texture>(image);
	}

	LevelRenderer::~LevelRenderer()
	{
		this->exit = true;

		mesh_thread.join();
	}

	void LevelRenderer::SetLevel(Level* level)
	{
		this->level = level;
	}

	void LevelRenderer::MeshChunk(ChunkPosition chunkPosition)
	{
		chunks_to_mesh.enqueue(chunkPosition);
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

	void LevelRenderer::Update()
	{
		ChunkPosition position;

		if (meshes_to_setup.try_dequeue(position))
		{
			if (!chunk_renderers.contains(position))
			{
				EON_ERROR("POWPDOAW");
			}
			else
			{
				chunk_renderers[position]->Setup();
			}
		}
	}

	void LevelRenderer::Render(Player& player)
	{
		if (level == nullptr)
		{
			return;
		}

		glm::vec4 skyColor = level->SkyColor();
		glClearColor(skyColor.x, skyColor.y, skyColor.z, skyColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		chunk_texture->Bind();

		chunk_shader->Bind();
		chunk_shader->UniformFVec4("fog_color", level->SkyColor());
		player.GetCamera().CalculateViewMatrix(glm::vec3(0, 0, 0));
		chunk_shader->UniformMatrix4("view", player.GetCamera().ViewMatrix());
		chunk_shader->UniformMatrix4("projection", player.GetCamera().ProjectionMatrix());
		chunk_shader->UniformFVec3("camPos", player.Position());

		player.GetCamera().CalculateViewMatrix(player.Position());

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			if (player.GetCamera().GetFrustum().BoxInFrustum(chunkRenderer->GetAABB()))
			{
				chunk_shader->UniformIVec3("chunkPos", glm::ivec3(chunkPosition.x * 16, 0, chunkPosition.z * 16));

				chunkRenderer->Render();
			}
		}
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

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void LevelRenderer::BuildChunkMesh(ChunkPosition inChunkPosition)
	{
		Chunk* chunk = level->GetChunk(inChunkPosition).value();
		glm::ivec3 chunkPosition(chunk->Position().x * 16, 0, chunk->Position().z * 16);
		ChunkMeshData meshData{};

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				for (int y = 0; y < 512; y++)
				{
					int numFaces = 0;
					glm::ivec3 position(x, y, z);
					auto block = chunk->GetBlock(x, y, z);

					if (!block.has_value())
					{
						EON_ERROR("Block was nullptr");
						break;
					}

					if (block.value()->type == BlockType::AIR)
					{
						continue;
					}

					Directions dir = Directions::Left;
					if (x > 0)
					{
						if (chunk->GetBlock(x - 1, y, z).value()->type == BlockType::AIR)
						{
							AddFace(meshData, position, block.value()->type, dir);
							numFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x - 1, y, z));
						if (sideBlock.has_value())
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block.value()->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Right;
					if (x < 15)
					{
						if (chunk->GetBlock(x + 1, y, z).value()->type == BlockType::AIR)
						{
							AddFace(meshData, position, block.value()->type, dir);
							numFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x + 1, y, z));
						if (sideBlock.has_value())
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block.value()->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Top;
					if (y < 511)
					{
						if (chunk->GetBlock(x, y + 1, z).value()->type == BlockType::AIR)
						{
							AddFace(meshData, position, block.value()->type, dir);
							numFaces++;
						}
					}
					else
					{
						AddFace(meshData, position, block.value()->type, dir);
						numFaces++;
					}

					dir = Directions::Bottom;
					if (y > 0)
					{
						if (chunk->GetBlock(x, y - 1, z).value()->type == BlockType::AIR)
						{
							AddFace(meshData, position, block.value()->type, dir);
							numFaces++;
						}
					}
					else
					{
						AddFace(meshData, position, block.value()->type, dir);
						numFaces++;
					}

					dir = Directions::Front;
					if (z < 15)
					{
						if (chunk->GetBlock(x, y, z + 1).value()->type == BlockType::AIR)
						{
							AddFace(meshData, position, block.value()->type, dir);
							numFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x, y, z + 1));
						if (sideBlock.has_value())
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block.value()->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Back;
					if (z > 0)
					{
						if (chunk->GetBlock(x, y, z - 1).value()->type == BlockType::AIR)
						{
							AddFace(meshData, position, block.value()->type, dir);
							numFaces++;
						}
					}
					else
					{
						auto sideBlock = level->GetBlock(chunkPosition + glm::ivec3(x, y, z - 1));
						if (sideBlock.has_value())
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block.value()->type, dir);
								numFaces++;
							}
						}
					}

					AddIndices(meshData, numFaces);
				}
			}
		}

		if (chunk_renderers.contains(chunk->Position()))
		{
			chunk_renderers.erase(chunk->Position());
		}

		chunk_renderers[chunk->Position()] = std::make_unique<ChunkRenderer>(chunk, meshData);

		meshes_to_setup.enqueue(chunk->Position());
	}

	void LevelRenderer::AddFace(ChunkMeshData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction)
	{
		auto faceData = GetFaceDataFromDirection(direction);
		auto blockUVs = BlockTexture::GetUVCoordsFromBlockID(blockType);
		auto uvs = GetUVsFromCoordinates(blockUVs);

		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int x = faceData[index++] + blockPosition.x;
			int y = faceData[index++] + blockPosition.y;
			int z = faceData[index++] + blockPosition.z;

			meshData.vertexPositions.emplace_back(x, y, z);
			meshData.directions.push_back(static_cast<u8>(direction));
			meshData.light.push_back(15);
			meshData.uvs.push_back(uvs[direction][i]);
		}
	}

	std::array<u8, 12> LevelRenderer::GetFaceDataFromDirection(Directions dir)
	{
		switch (dir)
		{
		case Directions::Front:
			return std::array<u8, 12>({ 0, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1 });
		case Directions::Back:
			return std::array<u8, 12>({ 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0 });
		case Directions::Left:
			return std::array<u8, 12>({ 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0 });
		case Directions::Right:
			return std::array<u8, 12>({ 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1 });
		case Directions::Top:
			return std::array<u8, 12>({ 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0 });
		case Directions::Bottom:
			return std::array<u8, 12>({ 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1 });
		}

		return {};
	}

	const std::map<Directions, std::vector<glm::vec2>> LevelRenderer::GetUVsFromCoordinates(std::map<const Directions, const glm::vec2>& coords)
	{
		std::map<Directions, std::vector<glm::vec2>> faceData;
		constexpr int TEX_SIZE = 16;

		for (const auto& faceCoord : coords)
		{
			faceData[faceCoord.first] =
			{
				glm::vec2((faceCoord.second.x + 1) / TEX_SIZE, (faceCoord.second.y + 1) / TEX_SIZE),
				glm::vec2(faceCoord.second.x / TEX_SIZE, (faceCoord.second.y + 1) / TEX_SIZE),
				glm::vec2(faceCoord.second.x / TEX_SIZE, faceCoord.second.y / TEX_SIZE),
				glm::vec2((faceCoord.second.x + 1) / TEX_SIZE, faceCoord.second.y / TEX_SIZE)
			};
		}

		return faceData;
	}

	void LevelRenderer::AddIndices(ChunkMeshData& meshData, int count)
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
}  // namespace Eon