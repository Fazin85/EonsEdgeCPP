#include "level_renderer.h"
#include "log.h"

namespace Eon
{
	LevelRenderer::LevelRenderer()
	{
		level = nullptr;
		mesh_thread = std::thread();
	}

	void LevelRenderer::SetLevel(Level* level)
	{
		this->level = level;
	}

	void LevelRenderer::MeshChunk(Chunk* chunk)
	{
		chunk_push_mutex.lock();
		chunks_to_mesh.push(chunk);
		chunk_push_mutex.unlock();
	}

	void LevelRenderer::MeshThread()
	{
		while (true)
		{
			chunk_pop_mutex.lock();
			if (chunks_to_mesh.size() > 0)
			{
				Chunk* chunk = chunks_to_mesh.front();
				chunks_to_mesh.pop();
				chunk_pop_mutex.unlock();

				BuildChunkMesh(chunk);
			}
			else
			{
				chunk_pop_mutex.unlock();
			}
		}
	}

	void LevelRenderer::BuildChunkMesh(Chunk* chunk)
	{
		glm::ivec3 chunkPosition(chunk->Position().x, 0, chunk->Position().z);
		std::vector<glm::ivec3> vertexData;

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

					if (block->type == 0)
					{
						continue;
					}

					Directions dir = Directions::Left;
					glm::ivec3 sidePosition(x - 1, y, z);
					if (x > 0)
					{
						if (chunk->GetBlock(sidePosition.x, sidePosition.y, sidePosition.z) == 0)
						{
							AddFace(vertexData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == 0)
							{
								AddFace(vertexData, position, block->type, dir);
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
							AddFace(vertexData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == 0)
							{
								AddFace(vertexData, position, block->type, dir);
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
							AddFace(vertexData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == 0)
							{
								AddFace(vertexData, position, block->type, dir);
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
							AddFace(vertexData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == 0)
							{
								AddFace(vertexData, position, block->type, dir);
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
							AddFace(vertexData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == 0)
							{
								AddFace(vertexData, position, block->type, dir);
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
							AddFace(vertexData, position, block->type, dir);
							numFaces++;
						}
					}
					else
					{
						Block* sideBlock = level->GetBlock(chunkPosition + sidePosition);
						if (sideBlock != nullptr)
						{
							if (sideBlock->type == 0)
							{
								AddFace(vertexData, position, block->type, dir);
								numFaces++;
							}
						}
					}
				}
			}
		}
	}

	void LevelRenderer::AddFace(std::vector<glm::ivec3>& vertexData, const glm::ivec3& blockPosition, u8 blockId, Directions direction)
	{
		std::array<u8, 12> faceData = GetFaceDataFromDirection(direction);

		int index = 0;
		for (int i = 0; i < 4; i++)
		{
			int x = faceData[index++] + blockPosition.x;
			int y = faceData[index++] + blockPosition.y;
			int z = faceData[index++] + blockPosition.z;

			vertexData.emplace_back(x, y, z);
		}
	}

	std::array<u8, 12> LevelRenderer::GetFaceDataFromDirection(Directions dir)
	{
		return std::array<u8, 12>();
	}
}  // namespace Eon