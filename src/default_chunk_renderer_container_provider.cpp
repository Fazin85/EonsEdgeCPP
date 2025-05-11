#include "default_chunk_renderer_container_provider.h"
#include "log.h"

namespace Eon
{
	DefaultChunkRendererContainerProvider::DefaultChunkRendererContainerProvider(Level& level) : level(level)
	{
	}

	std::unique_ptr<ChunkRendererContainer> DefaultChunkRendererContainerProvider::ProvideRenderer(ChunkPosition inChunkPosition)
	{
		auto chunk = level.GetChunk(inChunkPosition);

		if (!chunk.has_value())
		{
			//TODO: This isn't actually an error, if it happens more we should just remove it.
			std::stringstream ss;
			ss << "ProvideRenderer::Failed to get chunk at " << inChunkPosition.x << "," << inChunkPosition.z << "\n";
			throw std::runtime_error(ss.str());
		}

		glm::ivec3 chunkPosition(inChunkPosition.x, 0, inChunkPosition.z);

		ChunkMeshConstructionData opaqueMeshData{};
		ChunkMeshConstructionData cutoutMeshData{};
		ChunkMeshConstructionData translucentMeshData{};

		int lowest = CalculateLowestPoint(inChunkPosition, chunk.value()->GetChunkHeights().lowest);

		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				for (int y = lowest; y <= chunk.value()->GetColumnHeights(x, z).highest; y++)
				{
					glm::ivec3 position(x, y, z);
					const Block& block = chunk.value()->GetBlock(x, y, z);

					if (block.GetType() == BlockType::AIR)
					{
						continue;
					}

					auto& currentMeshData = GetMeshData(block, opaqueMeshData, cutoutMeshData, translucentMeshData);
					currentMeshData.face_count = 0;

					Directions dir = Directions::Left;

					if (x > 0)
					{
						auto sideBlock = chunk.value()->GetBlock(x - 1, y, z);
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x - 1, y, z));
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}

					dir = Directions::Right;
					if (x < CHUNK_WIDTH - 1)
					{
						auto sideBlock = chunk.value()->GetBlock(x + 1, y, z);
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x + 1, y, z));
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}

					dir = Directions::Top;
					if (y < CHUNK_HEIGHT - 1)
					{
						auto sideBlock = chunk.value()->GetBlock(x, y + 1, z);
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}
					else
					{
						AddFace(currentMeshData, position, block, dir);
					}

					dir = Directions::Bottom;
					if (y > 0)
					{
						auto sideBlock = chunk.value()->GetBlock(x, y - 1, z);
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}

					dir = Directions::Front;
					if (z < CHUNK_WIDTH - 1)
					{
						auto sideBlock = chunk.value()->GetBlock(x, y, z + 1);
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x, y, z + 1));
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}

					dir = Directions::Back;
					if (z > 0)
					{
						auto sideBlock = chunk.value()->GetBlock(x, y, z - 1);
						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}
					else
					{
						auto sideBlock = level.GetBlock(chunkPosition + glm::ivec3(x, y, z - 1));

						if (sideBlock.IsCutout() || (!block.Translucent() && sideBlock.Translucent()))
						{
							AddFace(currentMeshData, position, block, dir);
						}
					}

					if (currentMeshData.face_count > 0)
					{
						AddIndices(currentMeshData);
					}
				}
			}
		}

		auto opaqueChunkRenderer = std::make_unique<ChunkRenderer>(opaqueMeshData);
		auto chunkRendererContainer = std::make_unique<ChunkRendererContainer>(*chunk, std::move(opaqueChunkRenderer));

		if (!cutoutMeshData.vertexPositions.empty())
		{
			chunkRendererContainer->SetCutoutRenderer(std::make_unique<ChunkRenderer>(cutoutMeshData));
		}

		if (!translucentMeshData.vertexPositions.empty())
		{
			chunkRendererContainer->SetTranslucentRenderer(std::make_unique<ChunkRenderer>(translucentMeshData));
		}

		return chunkRendererContainer;
	}

	int DefaultChunkRendererContainerProvider::CalculateLowestPoint(const ChunkPosition& position, int middleLowest)
	{
		std::array<ChunkPosition, 4> positionsToCheck = {
			position.Offset(CHUNK_WIDTH, 0),
			position.Offset(0, CHUNK_WIDTH),
			position.Offset(-CHUNK_WIDTH, 0),
			position.Offset(0, -CHUNK_WIDTH) };

		auto chunks = level.GetChunks(positionsToCheck);

		if (chunks.size() != positionsToCheck.size())
		{
			EON_WARN("Failed to get all chunks for lowest point check");
			return 0;
		}

		int lowest = middleLowest;

		for (const auto& chunk : chunks)
		{
			int chunkLowest = chunk->GetChunkHeights().lowest;

			if (chunkLowest < lowest)
			{
				lowest = chunkLowest;
			}
		}

		return lowest;
	}

	ChunkMeshConstructionData& DefaultChunkRendererContainerProvider::GetMeshData(Block block, ChunkMeshConstructionData& opaqueMeshData, ChunkMeshConstructionData& cutoutMeshData, ChunkMeshConstructionData& translucentMeshData)
	{
		if (block.IsCutout())
		{
			return cutoutMeshData;
		}
		else if (block.Translucent())
		{
			return translucentMeshData;
		}

		return opaqueMeshData;
	}

	void DefaultChunkRendererContainerProvider::AddFace(ChunkMeshConstructionData& meshData, const glm::ivec3& blockPosition, const Block& block, Directions direction) const
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
			meshData.uvs.push_back({ i, GetTextureId(block.GetType(), direction) });
		}

		meshData.face_count++;
	}

	std::array<unsigned char, 12> DefaultChunkRendererContainerProvider::GetFaceDataFromDirection(Directions dir) const
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

	void DefaultChunkRendererContainerProvider::AddIndices(ChunkMeshConstructionData& meshData) const
	{
		for (int i = 0; i < meshData.face_count; i++)
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

	BlockFaceTexture DefaultChunkRendererContainerProvider::GetTextureId(BlockType blockType, Directions faceDirection) const
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
		case Eon::BlockType::OAK_LOG:
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
