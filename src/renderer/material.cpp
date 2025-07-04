#include "material.h"
#include "render_state.h"

namespace Eon
{
	Material::Material(TextureID texture, ShaderID shader, TransparencyType transparencyType) :
		texture(texture),
		shader(shader),
		transparency_type(transparencyType)
	{
	}

	void Material::SetPreRender(std::function<void(RenderState&)>& preRender)
	{
		pre_render = &preRender;
	}

	void Material::SetTexture(TextureID texture)
	{
		this->texture = texture;
	}

	void Material::SetShader(ShaderID shader)
	{
		this->shader = shader;
	}

	void Material::SetTransparencyType(TransparencyType transparencyType)
	{
		transparency_type = transparencyType;
	}

	TextureID Material::GetTexture() const
	{
		return texture;
	}

	ShaderID Material::GetShader() const
	{
		return shader;
	}

	TransparencyType Material::GetTransparencyType() const
	{
		return transparency_type;
	}

	void Material::PreRender(RenderState& renderState) const
	{
		if (pre_render)
		{
			const auto& preRender = *pre_render;
			preRender(renderState);
		}
	}
}
