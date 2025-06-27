#pragma once

#include "scene.h"
#include "billboard_sprite.h"
#include "level.h"
#include "level_renderer.h"
#include "plane_mesh.h"
#include "player.h"
#include "skybox.h"
#include "mesh.h"

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
		std::unique_ptr<BillboardSprite> sprite;
		std::unique_ptr<Skybox> skybox;
		std::unique_ptr<PositionTextureColorNormalMesh> cube;
		std::unique_ptr<Shader> ptcn_shader;
	};
}

