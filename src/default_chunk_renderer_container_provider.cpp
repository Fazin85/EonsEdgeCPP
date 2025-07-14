#include "default_chunk_renderer_container_provider.h"
#include "log.h"
#include "mesh.h"
#include "block_texture_provider.h"
#include "scope_timer.h"

namespace Eon
{
	DefaultChunkRendererContainerProvider::DefaultChunkRendererContainerProvider(Level& level, BlockTextureProvider& uvProvider) : level(level), uvProvider(uvProvider)
	{
	}

	std::unique_ptr<ChunkRendererContainer> DefaultChunkRendererContainerProvider::ProvideRenderer(ChunkRendererCreationContext& creationContext)
	{
		auto& chunk = creationContext.chunk;

		ChunkMeshConstructionData opaqueMeshData{};
		ChunkMeshConstructionData cutoutMeshData{};
		ChunkMeshConstructionData translucentMeshData{};

		int lowest = CalculateLowestPoint(chunk->Position(), chunk->GetChunkHeights().lowest);

		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_WIDTH; z++)
			{
				for (int y = lowest; y <= chunk->GetColumnHeights(x, z).highest; y++)
				{
					glm::ivec3 position(x, y, z);
					const Block& block = creationContext.blockProvider.GetBlock(x, y, z);

					if (block.GetID() == 0)
					{
						continue;
					}

					const Block& topBlock = y < CHUNK_HEIGHT - 1 ? creationContext.blockProvider.GetBlock(x, y + 1, z) : BlockRegistry::GetBlockByID(0);
					const Block& bottomBlock = y > 0 ? creationContext.blockProvider.GetBlock(x, y - 1, z) : BlockRegistry::GetBlockByID(1);
					const Block& leftBlock = creationContext.blockProvider.GetBlock(x - 1, y, z);
					const Block& rightBlock = creationContext.blockProvider.GetBlock(x + 1, y, z);
					const Block& frontBlock = creationContext.blockProvider.GetBlock(x, y, z + 1);
					const Block& backBlock = creationContext.blockProvider.GetBlock(x, y, z - 1);

					auto& currentMeshData = GetMeshData(block, opaqueMeshData, cutoutMeshData, translucentMeshData);

					BlockRenderContext renderContext
					{
						*chunk,
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
			std::move(meshData.normals));
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
