#pragma once

#include "scene.h"
#include "level.h"
#include "level_renderer.h"
#include "plane_mesh.h"
#include "player.h"
#include "skybox.h"
#include "mesh.h"
#include "texture_atlas_stitcher.h"
#include "framebuffer.h"
#include "renderer/default/default_render_pipeline.h"
#include "renderer/render_command_pool.h"

namespace Eon
{
	class InGameScene : public Scene
	{
	public:
		InGameScene();

		void Update(float dt) override;
		void Render() override;

		const char* GetName() override;

	private:
		std::unique_ptr<Player> player;
		std::unique_ptr<Level> level;
		std::unique_ptr<LevelRenderer> level_renderer;
		std::unique_ptr<Skybox> skybox;
		std::unique_ptr<PositionTextureColorNormalMesh> cube;
		std::unique_ptr<Shader> ptcn_shader;
		std::unique_ptr<Framebuffer> framebuffer;
		std::unique_ptr<RenderCommandPool> command_pool;
		std::unique_ptr<DefaultRenderPipeline> render_pipeline;
	};
}

