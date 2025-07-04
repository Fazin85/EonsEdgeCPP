#pragma once

#include "../asset_manager.h"
#include <functional>

namespace Eon
{
	enum class TransparencyType
	{
		Opaque,
		Cutout,
		Transparent
	};

	class RenderState;

	class Material
	{
	public:
		explicit Material(TextureID texture, ShaderID shader, TransparencyType type);

		void SetPreRender(std::function<void(RenderState&)>& preRender);
		void SetTexture(TextureID texture);
		void SetShader(ShaderID shader);
		void SetTransparencyType(TransparencyType transparencyType);

		TextureID GetTexture() const;
		ShaderID GetShader() const;
		TransparencyType GetTransparencyType() const;
		
		void PreRender(RenderState& renderState) const;

	private:
		std::function<void(RenderState&)>* pre_render = nullptr;
		TextureID texture;
		ShaderID shader;
		TransparencyType transparency_type;
	};
}