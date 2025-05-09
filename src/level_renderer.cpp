#include "block_texture.h"
#include "compress.h"
#include "level_renderer.h"
#include "log.h"
#include "settings.h"
#include <lz4.h>
#include <zlib.h>

namespace Eon
{
	LevelRenderer::LevelRenderer(Level& level, std::unique_ptr<ChunkRendererContainerProvider> chunkRendererContainerProvider) : level(level), chunk_renderer_container_provider(std::move(chunkRendererContainerProvider))
	{
		exit = false;

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
		chunk_shader->UniformFloat("fog_near", (static_cast<float>(GameSettings.render_distance) * static_cast<float>(CHUNK_WIDTH) / 2) + CHUNK_WIDTH);
		chunk_shader->UniformInt1("textureSampler", 0);

		int fogFar = GameSettings.fog ? GameSettings.render_distance * CHUNK_WIDTH : 100000;
		chunk_shader->UniformFloat("fog_far", static_cast<float>(fogFar));

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
	}

	void LevelRenderer::MeshChunk(ChunkPosition chunkPosition)
	{
		chunks_to_mesh.enqueue(chunkPosition);
		chunks_to_mesh_vector.push_back(chunkPosition);
	}

	void LevelRenderer::RemoveMesh(ChunkPosition chunkPosition)
	{
		if (chunk_renderers.contains(chunkPosition))
		{
			chunk_renderers.erase(chunkPosition);
		}
	}

	void LevelRenderer::Update(const Frustum& frustum, glm::vec3 cameraPosition)
	{
		std::unique_ptr<ChunkRendererContainer> chunk;
		ChunkPosition position;

		if (meshes_to_setup.try_dequeue(chunk))
		{
			position = chunk->GetChunk()->Position();

			if (chunk_renderers.contains(position))
			{
				chunk_renderers.erase(position);
			}

			chunk_renderers[position] = std::move(chunk);
			chunk_renderers[position]->Setup();

			std::erase(chunks_to_mesh_vector, position);
			MarkCanUnloadForMeshing(position, true);
		}

		auto playerChunkPosition = glm::vec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

		for (int cx = static_cast<int>(playerChunkPosition.x) - (CHUNK_WIDTH * GameSettings.render_distance); cx <= static_cast<int>(playerChunkPosition.x) + (CHUNK_WIDTH * GameSettings.render_distance); cx += CHUNK_WIDTH)
		{
			for (int cz = static_cast<int>(playerChunkPosition.z) - (CHUNK_WIDTH * GameSettings.render_distance); cz <= static_cast<int>(playerChunkPosition.z) + (CHUNK_WIDTH * GameSettings.render_distance); cz += CHUNK_WIDTH)
			{
				ChunkPosition currentChunkPosition{ cx, cz };

				if (CanChunkBeMeshed(currentChunkPosition, frustum))
				{
					MarkCanUnloadForMeshing(currentChunkPosition, false);
					MeshChunk(currentChunkPosition);
				}
			}
		}
	}

	void LevelRenderer::Render(Camera& camera, glm::vec3 cameraPosition)
	{
		glClearColor(level.SkyColor().r, level.SkyColor().g, level.SkyColor().b, level.SkyColor().a);

		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		chunk_texture->Bind();

		chunk_shader->Bind();
		chunk_shader->UniformFVec4("fog_color", level.SkyColor());
		camera.CalculateViewMatrix(glm::vec3(0, 0, 0));
		chunk_shader->UniformMatrix4("view", *camera.ViewMatrix());
		chunk_shader->UniformMatrix4("projection", *camera.ProjectionMatrix());
		chunk_shader->UniformFVec3("camPos", cameraPosition);

		camera.CalculateViewMatrix(cameraPosition);

		auto playerChunkPosition = glm::vec3((static_cast<int>(cameraPosition.x) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH, 0, (static_cast<int>(cameraPosition.z) >> CHUNK_BITSHIFT_AMOUNT) * CHUNK_WIDTH);

		static std::vector<std::pair<ChunkRendererContainer*, float>> cutoutRenderers;
		static std::vector<std::pair<ChunkRendererContainer*, float>> translucentRenderers;
		cutoutRenderers.clear();
		translucentRenderers.clear();

		std::array<int, 3> drawCalls = { 0 };

		for (const auto& [chunkPosition, chunkRenderer] : chunk_renderers)
		{
			float distance = glm::distance(playerChunkPosition, glm::vec3(chunkPosition.x, 0, chunkPosition.z));

			if (distance > static_cast<float>(GameSettings.render_distance * CHUNK_WIDTH) ||
				!camera.GetFrustum().BoxInFrustum(chunkRenderer->GetChunk()->GetAABB()))
			{
				continue;
			}

			auto cutoutRenderer = chunkRenderer->GetCutoutRenderer();
			auto translucentRenderer = chunkRenderer->GetTranslucentRenderer();

			if (cutoutRenderer.has_value())
			{
				cutoutRenderers.push_back({ chunkRenderer.get(), distance });
			}

			if (translucentRenderer.has_value())
			{
				translucentRenderers.push_back({ chunkRenderer.get(), distance });
			}

			chunk_shader->UniformFVec3("chunkPos", glm::vec3(chunkPosition.x, 0.0f, chunkPosition.z));

			chunkRenderer->GetOpaqueRenderer().Render();
			drawCalls[0]++;
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
			std::cout << "odc:" << drawCalls[0] << ", cdc:" << drawCalls[1] << ", tdc:" << drawCalls[2] << "\n";
		}
	}

	size_t LevelRenderer::ChunkRendererCount()
	{
		return chunk_renderers.size();
	}

	bool LevelRenderer::IsChunkBeingMeshed(ChunkPosition position)
	{
		return std::ranges::find(chunks_to_mesh_vector, position) != chunks_to_mesh_vector.end();
	}

	void LevelRenderer::OnChunkUnloaded(std::shared_ptr<Chunk> chunk)
	{
		RemoveMesh(chunk->Position());
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

	void LevelRenderer::MeshThread()
	{
		while (!exit)
		{
			ChunkPosition chunkPosition;
			if (chunks_to_mesh.try_dequeue(chunkPosition))
			{
				meshes_to_setup.enqueue(chunk_renderer_container_provider->ProvideRenderer(chunkPosition));
			}
		}
	}

	bool LevelRenderer::CanChunkBeMeshed(ChunkPosition position, const Frustum& frustum)
	{
		auto chunk = level.GetChunk(position);

		if (!chunk)
		{
			return false;
		}

		bool flag = !chunk_renderers.contains(position) && !IsChunkBeingMeshed(position) && frustum.BoxInFrustum(chunk.value()->GetAABB());

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

			if (chunk.has_value())
			{
				chunk.value()->SetCanUnload(canUnload);
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