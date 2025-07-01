#include "default_chunk_renderer_container_provider.h"
#include "log.h"
#include "mesh.h"
#include "block_texture_provider.h"

namespace Eon
{
	DefaultChunkRendererContainerProvider::DefaultChunkRendererContainerProvider(Level& level, BlockTextureProvider& uvProvider) : level(level), uvProvider(uvProvider)
	{
	}

	std::unique_ptr<ChunkRendererContainer> DefaultChunkRendererContainerProvider::ProvideRenderer(ChunkPosition inChunkPosition)
	{
		auto chunk = level.GetChunk(inChunkPosition);

		if (!chunk)
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

		int lowest = CalculateLowestPoint(inChunkPosition, chunk->GetChunkHeights().lowest);

		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				for (int y = lowest; y <= chunk->GetColumnHeights(x, z).highest; y++)
				{
					glm::ivec3 position(x, y, z);
					const Block& block = chunk->GetBlock(x, y, z);

					if (block.GetID() == 0)
					{
						continue;
					}

					const Block& topBlock = y < CHUNK_HEIGHT - 1 ? chunk->GetBlock(x, y + 1, z) : BlockRegistry::GetBlockByID(0);
					const Block& bottomBlock = y > 0 ? chunk->GetBlock(x, y - 1, z) : BlockRegistry::GetBlockByID(1);
					const Block& leftBlock = x > 0 ? chunk->GetBlock(x - 1, y, z) : level.GetBlock(chunkPosition + glm::ivec3(x - 1, y, z));
					const Block& rightBlock = x < CHUNK_WIDTH - 1 ? chunk->GetBlock(x + 1, y, z) : level.GetBlock(chunkPosition + glm::ivec3(x + 1, y, z));
					const Block& frontBlock = z > 0 ? chunk->GetBlock(x, y, z + 1) : level.GetBlock(chunkPosition + glm::ivec3(x, y, z + 1));
					const Block& backBlock = z < CHUNK_WIDTH - 1 ? chunk->GetBlock(x, y, z - 1) : level.GetBlock(chunkPosition + glm::ivec3(x, y, z - 1));

					auto& currentMeshData = GetMeshData(block, opaqueMeshData, cutoutMeshData, translucentMeshData);

					BlockRenderContext renderContext
					{
						position,
						block,
						topBlock,
						bottomBlock,
						leftBlock,
						rightBlock,
						frontBlock,
						backBlock,
						currentMeshData,
						uvProvider
					};

					block.Render(renderContext);
				}
			}
		}

		auto opaqueChunkRenderer = CreateMesh(opaqueMeshData);
		auto chunkRendererContainer = std::make_unique<ChunkRendererContainer>(chunk, std::move(opaqueChunkRenderer));

		if (!cutoutMeshData.vertexPositions.empty())
		{
			chunkRendererContainer->SetCutoutRenderer(CreateMesh(cutoutMeshData));
		}

		if (!translucentMeshData.vertexPositions.empty())
		{
			chunkRendererContainer->SetTranslucentRenderer(CreateMesh(translucentMeshData));
		}

		return chunkRendererContainer;
	}

	std::unique_ptr<Mesh> DefaultChunkRendererContainerProvider::CreateMesh(ChunkMeshConstructionData& meshData)
	{
		return std::make_unique<PositionTextureColorNormalMesh>(
			std::move(meshData.vertexPositions),
			std::move(meshData.uvs),
			std::move(meshData.colors),
			std::move(meshData.normals),
			AssetID::INVALID_ASSET_ID);
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
}
