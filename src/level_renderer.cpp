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

	void LevelRenderer::MeshChunk(Chunk* chunk)
	{
		chunks_to_mesh.enqueue(chunk);
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

	void LevelRenderer::MeshThread()
	{
		while (!exit)
		{
			Chunk* chunk = nullptr;
			if (chunks_to_mesh.try_dequeue(chunk))
			{
				BuildChunkMesh(chunk);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void LevelRenderer::BuildChunkMesh(Chunk* chunk)
	{
		glm::ivec3 chunkPosition(chunk->Position().x, 0, chunk->Position().z);
		ChunkMeshData meshData{};

		for (int x = 0; x < 16; x++)
		{
			for (int z = 0; z < 16; z++)
			{
				for (int y = 0; y < 512; y++)
				{
					int numFaces = 0;
					glm::ivec3 position(x, y, z);
					Block* block = chunk->GetBlock(x, y, z);

					if (block == nullptr)
					{
						EON_ERROR("Block was nullptr");
						break;
					}

					if (block->type == BlockType::AIR)
					{
						continue;
					}

					Directions dir = Directions::Left;
					glm::ivec3 sidePosition(x - 1, y, z);
					if (x > 0)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(meshData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Right;
					sidePosition.x = x + 1;
					sidePosition.y = y;
					sidePosition.z = z;
					if (x < 15)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(meshData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Top;
					sidePosition.x = x;
					sidePosition.y = y + 1;
					sidePosition.z = z;
					if (x < 511)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(meshData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Bottom;
					sidePosition.x = x;
					sidePosition.y = y - 1;
					sidePosition.z = z;
					if (x > 0)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(meshData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Front;
					sidePosition.x = x;
					sidePosition.y = y;
					sidePosition.z = z + 1;
					if (z < 15)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(meshData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block->type, dir);
								numFaces++;
							}
						}
					}

					dir = Directions::Back;
					sidePosition.x = x;
					sidePosition.y = y;
					sidePosition.z = z - 1;
					if (z > 0)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(meshData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == BlockType::AIR)
							{
								AddFace(meshData, position, block->type, dir);
								numFaces++;
							}
						}
					}

					AddIndices(meshData, numFaces);
				}
			}
		}

		meshes_to_setup.enqueue(chunk->Position());

		if (chunk_renderers.contains(chunk->Position()))
		{
			chunk_renderers.erase(chunk->Position());
		}

		chunk_renderers[chunk->Position()] = std::make_unique<ChunkRenderer>(chunk, meshData);
	}

	void LevelRenderer::AddFace(ChunkMeshData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction)
	{
		std::array<u8, 12> faceData = GetFaceDataFromDirection(direction);

		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int x = faceData[index++] + blockPosition.x;
			int y = faceData[index++] + blockPosition.y;
			int z = faceData[index++] + blockPosition.z;

			meshData.vertexPositions.emplace_back(x, y, z);
			meshData.directions.push_back(static_cast<u8>(direction));
		}

		auto blockUVs = BlockTexture::GetUVCoordsFromBlockID(blockType);
		auto realUVs = GetUVsFromCoordinates(blockUVs);

		for (const glm::vec2 uv : realUVs[direction])
		{
			meshData.uvs.push_back(uv);
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