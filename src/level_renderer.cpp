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
		/*auto chunk_shader = AssetManager::GetAsset<Shader>("shader.chunk");

		chunk_shader->Bind();
		chunk_shader->UniformFVec3("sunlight_dir", glm::vec3(-0.7f, -1.0f, -0.5f));
		chunk_shader->UniformFVec3("light_color", glm::vec3(1, 1, 1));
		chunk_shader->UniformFloat("ambient_light", 0.15f);
		chunk_shader->UniformMatrix4Transpose("model", glm::mat4(1.0f));
		chunk_shader->UniformFloat("fog_near", (static_cast<float>(GameSettings.render_distance) * static_cast<float>(CHUNK_WIDTH) / 2) + CHUNK_WIDTH);
		chunk_shader->UniformInt1("textureSampler", 0);

		int fogFar = GameSettings.fog ? GameSettings.render_distance * CHUNK_WIDTH : 100000;
		chunk_shader->UniformFloat("fog_far", static_cast<float>(fogFar));

		TextureAtlasStitcher stitcher;
		stitcher.AddSprite("Error.png");

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

		chunk_texture = std::make_unique<TextureArray>(imageNames, 16, 16);*/
	}

	LevelRenderer::~LevelRenderer()
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

	void LevelRenderer::Update(const Frustum& frustum, glm::vec3 cameraPosition)
	{
		ProcessSingleChunkMesh();

		auto playerChunkPosition = glm::vec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

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

	void LevelRenderer::Render(RenderPipeline& renderPipeline, RenderCommandPool& commandPool, Camera& camera, ChunkPosition playerChunkPosition)
	{
		//glClearColor(level.SkyColor().r, level.SkyColor().g, level.SkyColor().b, level.SkyColor().a);

		//glCullFace(GL_BACK);
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Material chunkMaterial{ AssetManager::GetAsset<Texture>("texture.block_atlas").GetID(), AssetManager::GetAsset<Shader>("shader.ptcn_blinn_phong").GetID(), TransparencyType::Opaque };

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			float distance = glm::distance(glm::vec3(playerChunkPosition.x, 0, playerChunkPosition.z), glm::vec3(chunkPosition.x, 0, chunkPosition.z));
			const auto& chunk = chunkRenderer->GetChunk();

			if (distance > static_cast<float>(GameSettings.render_distance * CHUNK_WIDTH) ||
				!camera.GetFrustum().BoxInFrustum(chunk->GetAABB()))
			{
				continue;
			}

			glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), glm::vec3(chunkPosition.x, 0, chunkPosition.z));

			auto& command = commandPool.CreateCommand<MeshRenderCommand>(chunkRenderer->GetOpaqueRenderer(), model, 0.0f, chunkMaterial);
			renderPipeline.Submit(command);
		}
		/*chunk_texture->Bind();

		auto chunk_shader = AssetManager::GetAsset<Shader>("shader.chunk");

		chunk_shader->Bind();
		chunk_shader->UniformFVec4("fog_color", level.SkyColor());
		camera.CalculateViewMatrix(glm::vec3(0, 0, 0));
		chunk_shader->UniformMatrix4Transpose("view", camera.ViewMatrix());
		chunk_shader->UniformMatrix4Transpose("projection", camera.ProjectionMatrix());
		chunk_shader->UniformFVec3("camPos", cameraPosition);

		camera.CalculateViewMatrix(cameraPosition);

		auto playerChunkPosition = glm::vec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

		static std::vector<std::pair<ChunkRendererContainer*, float>> cutoutRenderers;
		static std::vector<std::pair<ChunkRendererContainer*, float>> translucentRenderers;
		static std::vector<BlockEntity*> blockEntitys;
		cutoutRenderers.clear();
		translucentRenderers.clear();
		blockEntitys.clear();

		std::array<int, 4> drawCalls = { 0 };

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			float distance = glm::distance(playerChunkPosition, glm::vec3(chunkPosition.x, 0, chunkPosition.z));
			const auto& chunk = chunkRenderer->GetChunk();

			if (distance > static_cast<float>(GameSettings.render_distance * CHUNK_WIDTH) ||
				!camera.GetFrustum().BoxInFrustum(chunk->GetAABB()))
			{
				continue;
			}

			const auto& chunkBlockEntitys = chunk->GetBlockEntitys();
			blockEntitys.insert(blockEntitys.end(), chunkBlockEntitys.begin(), chunkBlockEntitys.end());

			auto cutoutRenderer = chunkRenderer->GetCutoutRenderer();
			auto translucentRenderer = chunkRenderer->GetTranslucentRenderer();

			if (cutoutRenderer)
			{
				cutoutRenderers.emplace_back(chunkRenderer.get(), distance);
			}

			if (translucentRenderer)
			{
				translucentRenderers.emplace_back(chunkRenderer.get(), distance);
			}

			chunk_shader->UniformFVec3("chunkPos", glm::vec3(chunkPosition.x, 0.0f, chunkPosition.z));

			chunkRenderer->GetOpaqueRenderer().Render();
			drawCalls[0]++;
		}

		for (const auto& blockEntity : blockEntitys)
		{
			blockEntity->Render(camera);
			drawCalls[3]++;
		}

		SortRenderersByDistance(cutoutRenderers, cameraPosition);
		SortRenderersByDistance(translucentRenderers, cameraPosition);

		for (const auto& [chunkRenderer, distance] : cutoutRenderers)
		{
			ChunkPosition chunkPosition = chunkRenderer->GetChunk()->Position();

			chunk_shader->UniformFVec3("chunkPos", glm::vec3(chunkPosition.x, 0.0f, chunkPosition.z));

			chunkRenderer->GetCutoutRenderer()->get().Render();
			drawCalls[1]++;
		}

		for (const auto& [chunkRenderer, distance] : translucentRenderers)
		{
			ChunkPosition chunkPosition = chunkRenderer->GetChunk()->Position();

			chunk_shader->UniformFVec3("chunkPos", glm::vec3(chunkPosition.x, 0.0f, chunkPosition.z));

			chunkRenderer->GetTranslucentRenderer()->get().Render();
			drawCalls[2]++;
		}

		static int timer = 0;
		timer++;
		if (timer % 200 == 0)
		{
			EON_INFO(
				"odc: " + std::to_string(drawCalls[0]) +
				", cdc: " + std::to_string(drawCalls[1]) +
				", tdc: " + std::to_string(drawCalls[2]) +
				", bedc: " + std::to_string(drawCalls[3]));
		}*/
	}

	size_t LevelRenderer::ChunkRendererCount()
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

	void LevelRenderer::SortRenderersByDistance(std::vector<std::pair<ChunkRendererContainer*, float>>& renderers, glm::vec3 cameraPosition) const
	{
		std::ranges::sort(renderers, [cameraPosition](std::pair<ChunkRendererContainer*, float> first, std::pair<ChunkRendererContainer*, float> second)
			{
				ChunkPosition firstPosition = first.first->GetChunk()->Position();
				ChunkPosition secondPosition = second.first->GetChunk()->Position();

				return glm::distance(cameraPosition, glm::vec3(firstPosition.x, 0, firstPosition.z)) > glm::distance(cameraPosition, glm::vec3(secondPosition.x, 0, secondPosition.z));
			});
	}

	void LevelRenderer::SortChunksByDistance(std::vector<ChunkPosition>& chunks, glm::vec3 cameraPosition) const
	{
		std::ranges::sort(chunks, [cameraPosition](const ChunkPosition& first, const ChunkPosition& second)
			{
				return glm::distance(cameraPosition, glm::vec3(first.x, 0, first.z)) < glm::distance(cameraPosition, glm::vec3(second.x, 0, second.z));
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