#include "default_aabb_chunk_renderer_provider.h"
#include "log.h"

namespace Eon {
	DefaultAABBChunkRendererProvider::DefaultAABBChunkRendererProvider(Level& level) : level(level)
	{
	}

	std::unique_ptr<AABBChunkRenderer> DefaultAABBChunkRendererProvider::ProvideRenderer(ChunkPosition inChunkPosition)
	{
		auto chunk = level.GetChunk(inChunkPosition);

		if (!chunk.has_value()) {
			std::stringstream ss{};
			ss << "Failed to get chunk at " << inChunkPosition.x << "," << inChunkPosition.z << "\n";
			throw std::runtime_error(ss.str());
		}

		glm::ivec3 chunkPosition(chunk->get().Position().x, 0, chunk->get().Position().z);
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
					short height = chunk->get().GetHeightestBlockY(x, z);
					bool currentTransparent = false;

					if (block.type == BlockType::AIR)
					{
						continue;
					}

					if (block.TransparentThick() || block.Transparent())
					{
						tranparency = true;
						currentTransparent = true;
					}

					BlockType type = block.type;

					Directions dir = Directions::Left;
					if (x > 0)
					{
						auto sideBlock = chunk->get().GetBlock(x - 1, y, z);
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x - 1, y, z));
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Right;
					if (x < CHUNK_WIDTH - 1)
					{
						auto sideBlock = chunk->get().GetBlock(x + 1, y, z);
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x + 1, y, z));
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Top;
					if (y < CHUNK_HEIGHT - 1)
					{
						auto sideBlock = chunk->get().GetBlock(x, y + 1, z);
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
						currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
					}

					dir = Directions::Bottom;
					if (y > 0)
					{
						auto sideBlock = chunk->get().GetBlock(x, y - 1, z);
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Front;
					if (z < CHUNK_WIDTH - 1)
					{
						auto sideBlock = chunk->get().GetBlock(x, y, z + 1);
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x, y, z + 1));
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}

					dir = Directions::Back;
					if (z > 0)
					{
						auto sideBlock = chunk->get().GetBlock(x, y, z - 1);
						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x, y, z - 1));

						if (sideBlock.Transparent() || (!block.TransparentThick() && sideBlock.TransparentThick()))
						{
							AddFace(currentTransparent ? transparentMeshData : opaqueMeshData, position, type, dir);
							currentTransparent ? numTransparentFaces++ : numOpaqueFaces++;
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
		if (tranparency)
		{
			chunkRenderer->SetWaterMesh(std::make_unique<ChunkRenderer>(transparentMeshData));
		}

		return std::make_unique<AABBChunkRenderer>(chunk->get(), std::move(chunkRenderer));
	}

	void DefaultAABBChunkRendererProvider::AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, BlockType blockType, Directions direction)
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

	std::array<unsigned char, 12> DefaultAABBChunkRendererProvider::GetFaceDataFromDirection(Directions dir)
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

	void DefaultAABBChunkRendererProvider::AddIndices(ChunkMeshConstructionData& meshData, int count)
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

	BlockFaceTexture DefaultAABBChunkRendererProvider::GetTextureId(BlockType blockType, Directions faceDirection)
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
}
