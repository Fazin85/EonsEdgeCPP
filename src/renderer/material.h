#pragma once

#include "../asset_manager.h"

namespace Eon
{
	enum class TransparencyType
	{
		Opaque,
		Cutout,
		Transparent
	};

	class Material
	{
	public:
		explicit Material(TextureID texture, ShaderID shader, TransparencyType);

		void SetTexture(TextureID texture);
		void SetShader(ShaderID shader);
		void SetTransparencyType(TransparencyType transparencyType);

		TextureID GetTexture() const;
		ShaderID GetShader() const;
		TransparencyType GetTransparencyType() const;

		void Bind() const;

	private:
		TextureID texture;
		ShaderID shader;
		TransparencyType transparency_type;
	};
}