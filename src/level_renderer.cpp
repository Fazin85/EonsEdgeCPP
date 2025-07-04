#include "compress.h"
#include "level_renderer.h"
#include "log.h"
#include "settings.h"
#include <lz4.h>
#include <zlib.h>
#include "asset_manager.h"
#include "texture_atlas_stitcher.h"
#include "block_texture_provider.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Eon
{
	LevelRenderer::LevelRenderer(Level& level, std::unique_ptr<ChunkRendererContainerProvider> chunkRendererContainerProvider)
		: chunk_renderer_container_provider(std::move(chunkRendererContainerProvider)), level(level)
	{
	}

	void LevelRenderer::MeshChunk(ChunkPosition chunkPosition)
	{
		if (std::ranges::find(chunks_to_mesh, chunkPosition) == chunks_to_mesh.end() &&
			!chunk_renderers.contains(chunkPosition))
		{
			chunks_to_mesh.push_back(chunkPosition);
		}
	}

	void LevelRenderer::RemoveMesh(ChunkPosition chunkPosition)
	{
		if (chunk_renderers.contains(chunkPosition))
		{
			chunk_renderers.erase(chunkPosition);
		}

		std::erase(chunks_to_mesh, chunkPosition);
	}

	void LevelRenderer::Update(const Frustum& frustum, glm::dvec3 cameraPosition)
	{
		ProcessSingleChunkMesh();

		auto playerChunkPosition = glm::dvec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

		for (int cx = static_cast<int>(playerChunkPosition.x) - (CHUNK_WIDTH * GameSettings.render_distance); cx <= static_cast<int>(playerChunkPosition.x) + (CHUNK_WIDTH * GameSettings.render_distance); cx += CHUNK_WIDTH)
		{
			for (int cz = static_cast<int>(playerChunkPosition.z) - (CHUNK_WIDTH * GameSettings.render_distance); cz <= static_cast<int>(playerChunkPosition.z) + (CHUNK_WIDTH * GameSettings.render_distance); cz += CHUNK_WIDTH)
			{
				ChunkPosition currentChunkPosition{ cx, cz };

				if (CanChunkBeMeshed(currentChunkPosition, &frustum))
				{
					MarkCanUnloadForMeshing(currentChunkPosition, false);
					MeshChunk(currentChunkPosition);
				}
			}
		}

		SortChunksByDistance(chunks_to_mesh, cameraPosition);
	}

	void LevelRenderer::Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, Camera& camera, glm::dvec3 cameraPosition) const
	{
		Material opaqueChunkMaterial{ AssetManager::GetAsset<Texture>("texture.block_atlas").GetID(), AssetManager::GetAsset<Shader>("shader.ptcn_blinn_phong_chunk").GetID(), TransparencyType::Opaque };
		Material cutoutChunkMaterial{ AssetManager::GetAsset<Texture>("texture.block_atlas").GetID(), AssetManager::GetAsset<Shader>("shader.ptcn_blinn_phong_chunk").GetID(), TransparencyType::Cutout };
		Material translucentChunkMaterial{ AssetManager::GetAsset<Texture>("texture.block_atlas").GetID(), AssetManager::GetAsset<Shader>("shader.ptcn_blinn_phong_chunk").GetID(), TransparencyType::Transparent };
		auto blockIDTexture = AssetManager::GetAsset<Texture>("texture.block_id_atlas");

		static std::function<void(RenderState&)> preRenderChunk = [blockIDTexture](RenderState& renderState)
			{
				renderState.SetTexture(blockIDTexture.GetID(), 1);
				renderState.Apply();
			};

		opaqueChunkMaterial.SetPreRender(preRenderChunk);

		static std::vector<std::pair<ChunkRendererContainer*, float>> cutoutRenderers;
		static std::vector<std::pair<ChunkRendererContainer*, float>> translucentRenderers;
		cutoutRenderers.clear();
		translucentRenderers.clear();

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			AABB aabb = chunkRenderer->GetChunk()->GetAABB();
			aabb.position -= cameraPosition;

			if (glm::distance(glm::dvec3(chunkPosition.x, 0.0, chunkPosition.z), glm::dvec3(chunkPosition.x, 0, chunkPosition.z)) > static_cast<double>(GameSettings.render_distance * CHUNK_WIDTH) ||
				!camera.GetFrustum().BoxInFrustum(aabb))
			{
				continue;
			}

			glm::dvec3 renderChunkPosition = glm::dvec3(chunkPosition.x, 0, chunkPosition.z) - cameraPosition;
			glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), glm::vec3
			(
				static_cast<float>(renderChunkPosition.x),
				static_cast<float>(renderChunkPosition.y),
				static_cast<float>(renderChunkPosition.z)
			));

			auto cutoutRenderer = chunkRenderer->GetCutoutRenderer();
			auto translucentRenderer = chunkRenderer->GetTranslucentRenderer();

			auto depth = static_cast<float>(glm::length(renderChunkPosition));

			if (cutoutRenderer)
			{
				auto& command = commandPool.CreateCommand<MeshRenderCommand>(*cutoutRenderer, model, depth, cutoutChunkMaterial);
				renderPipeline.Submit(command);
			}

			if (translucentRenderer)
			{
				auto& command = commandPool.CreateCommand<MeshRenderCommand>(*translucentRenderer, model, depth, translucentChunkMaterial);
				renderPipeline.Submit(command);
			}

			auto& command = commandPool.CreateCommand<MeshRenderCommand>(chunkRenderer->GetOpaqueRenderer(), model, depth, opaqueChunkMaterial);
			renderPipeline.Submit(command);
		}
	}

	size_t LevelRenderer::ChunkRendererCount() const
	{
		return chunk_renderers.size();
	}

	bool LevelRenderer::IsChunkBeingMeshed(ChunkPosition position)
	{
		return std::ranges::find(chunks_to_mesh, position) != chunks_to_mesh.end();
	}

	void LevelRenderer::OnChunkUnloaded(std::shared_ptr<Chunk> chunk)
	{
		RemoveMesh(chunk->Position());
	}

	void LevelRenderer::ProcessSingleChunkMesh()
	{
		if (chunks_to_mesh.empty())
		{
			return;
		}

		ChunkPosition chunkPosition = chunks_to_mesh.front();
		chunks_to_mesh.erase(chunks_to_mesh.begin());

		if (!CanChunkBeMeshed(chunkPosition, nullptr))
		{
			return;
		}

		std::unique_ptr<ChunkRendererContainer> chunkRenderer = chunk_renderer_container_provider->ProvideRenderer(chunkPosition);

		if (chunkRenderer)
		{
			if (chunk_renderers.contains(chunkPosition))
			{
				chunk_renderers.erase(chunkPosition);
			}

			chunk_renderers[chunkPosition] = std::move(chunkRenderer);
			chunk_renderers[chunkPosition]->Setup();

			MarkCanUnloadForMeshing(chunkPosition, true);
		}
	}

	void LevelRenderer::SortRenderersByDistance(std::vector<std::pair<ChunkRendererContainer*, float>>& renderers, glm::dvec3 cameraPosition) const
	{
		std::ranges::sort(renderers, [cameraPosition](std::pair<ChunkRendererContainer*, float> first, std::pair<ChunkRendererContainer*, float> second)
			{
				ChunkPosition firstPosition = first.first->GetChunk()->Position();
				ChunkPosition secondPosition = second.first->GetChunk()->Position();

				return glm::distance(cameraPosition, glm::dvec3(firstPosition.x, 0, firstPosition.z)) > glm::distance(cameraPosition, glm::dvec3(secondPosition.x, 0, secondPosition.z));
			});
	}

	void LevelRenderer::SortChunksByDistance(std::vector<ChunkPosition>& chunks, glm::dvec3 cameraPosition) const
	{
		std::ranges::sort(chunks, [cameraPosition](const ChunkPosition& first, const ChunkPosition& second)
			{
				return glm::distance(cameraPosition, glm::dvec3(first.x, 0, first.z)) < glm::distance(cameraPosition, glm::dvec3(second.x, 0, second.z));
			});
	}

	bool LevelRenderer::CanChunkBeMeshed(ChunkPosition position, const Frustum* frustum)
	{
		auto chunk = level.GetChunk(position);

		if (!chunk)
		{
			return false;
		}

		bool flag = !chunk_renderers.contains(position) && !IsChunkBeingMeshed(position);

		if (frustum != nullptr)
		{
			flag = flag && frustum->BoxInFrustum(chunk->GetAABB());
		}

		bool chunkExists1 = level.ChunkExistsAt(position.Offset(CHUNK_WIDTH, 0));
		bool chunkExists2 = level.ChunkExistsAt(position.Offset(0, CHUNK_WIDTH));
		bool chunkExists3 = level.ChunkExistsAt(position.Offset(-CHUNK_WIDTH, 0));
		bool chunkExists4 = level.ChunkExistsAt(position.Offset(0, -CHUNK_WIDTH));
		bool chunkExists5 = level.ChunkExistsAt(position.Offset(CHUNK_WIDTH, CHUNK_WIDTH));
		bool chunkExists6 = level.ChunkExistsAt(position.Offset(-CHUNK_WIDTH, CHUNK_WIDTH));
		bool chunkExists7 = level.ChunkExistsAt(position.Offset(CHUNK_WIDTH, -CHUNK_WIDTH));
		bool chunkExists8 = level.ChunkExistsAt(position.Offset(-CHUNK_WIDTH, -CHUNK_WIDTH));

		return flag && chunkExists1 && chunkExists2 && chunkExists3 && chunkExists4 && chunkExists5 && chunkExists6 && chunkExists7 && chunkExists8;
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

		for (const ChunkPosition& chunkPosition : positions)
		{
			auto chunk = level.GetChunk(chunkPosition);

			if (chunk)
			{
				chunk->SetCanUnload(canUnload);
			}
			else
			{
				std::stringstream ss;
				ss << "Failed to get chunk at: " << chunkPosition.x << "," << chunkPosition.z << "\n";
				EON_CRITICAL(ss.str());
			}
		}
	}
}  // namespace Eon