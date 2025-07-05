#include "render_state.h"
#include "../framebuffer.h"
#include "../log.h"
#include "../assert.h"

namespace Eon
{
	bool RenderState::SetShader(ShaderID shader)
	{
		if (shader != this->shader)
		{
			this->shader = shader;
			modified = true;
			return true;
		}
		return false;
	}

	bool RenderState::SetTexture(TextureID texture, int unit)
	{
		if (texture != bound_textures[unit])
		{
			bound_textures[unit] = texture;
			modified = true;
			return true;
		}
		return false;
	}

	Shader* RenderState::GetShader() const
	{
		return shader.IsValid() ? shader.Get<Shader>().Get() : nullptr;
	}

	Texture* RenderState::GetTexture(int unit) const
	{
		return bound_textures[unit].IsValid() ? bound_textures[unit].Get<Texture>().Get() : nullptr;
	}

	void RenderState::SetGBufferTextures(GLuint albedo, GLuint normal, GLuint position)
	{
		g_buffer_albedo = albedo;
		g_buffer_normal = normal;
		g_buffer_position = position;
	}

	void RenderState::SetSSRTexture(GLuint ssrTexture)
	{
		ssr_texture = ssrTexture;
	}

	void RenderState::SetFramebuffer(Framebuffer* frameBuffer)
	{
		if (frameBuffer != this->frame_buffer)
		{
			this->frame_buffer = frameBuffer;
			modified = true;
		}
	}

	void RenderState::SetSSBO(GLuint ssboId, GLuint bindingPoint)
	{
		if (ssboId != ssbo_id)
		{
			ssbo_id = ssboId;
			binding_point = bindingPoint;
			modified = true;
		}
	}

	void RenderState::SetDepthMask(bool depthMask)
	{
		if (depthMask != depth_mask)
		{
			depth_mask = depthMask;
			modified = true;
		}
	}

	void RenderState::SetDepthFunc(GLenum depthFunction)
	{
		if (depthFunction != depth_function)
		{
			depth_function = depthFunction;
			modified = true;
		}
	}

	void RenderState::SetDepthTest(bool depthTest)
	{
		if (depthTest != this->depth_test)
		{
			this->depth_test = depthTest;
			modified = true;
		}
	}

	void RenderState::SetBlend(bool blend)
	{
		if (blend != this->blend)
		{
			this->blend = blend;
			modified = true;
		}
	}

	void RenderState::SetBlendFunc(GLenum blendSrc, GLenum blendDst)
	{
		if (blendSrc != blend_src)
		{
			blend_src = blendSrc;
			modified = true;
		}

		if (blendDst != blend_dst)
		{
			blend_dst = blendDst;
			modified = true;
		}
	}

	void RenderState::SetCullFace(bool cullFace)
	{
		if (cullFace != cull_face)
		{
			cull_face = cullFace;
			modified = true;
		}
	}

	void RenderState::SetCullFaceMode(GLenum cullFaceMode)
	{
		if (cullFaceMode != cull_face_mode)
		{
			cull_face_mode = cullFaceMode;
			modified = true;
		}
	}

	void RenderState::BindGBufferTextures()
	{
		EON_ASSERT(g_buffer_albedo && g_buffer_normal && g_buffer_position, "gBuffer textures not set!");
		EON_ASSERT(g_buffer_albedo != g_buffer_normal, "gBuffer textures are the same!");
		EON_ASSERT(g_buffer_normal != g_buffer_position, "gBuffer textures are the same!");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_buffer_albedo);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, g_buffer_normal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_buffer_position);

		for (int i = 0; i < 3; i++)
		{
			bound_textures[i] = AssetID::INVALID_ASSET_ID;
		}
	}

	GLuint RenderState::GetSSRTexture() const
	{
		return ssr_texture;
	}

	Framebuffer* RenderState::GetFramebuffer() const
	{
		return frame_buffer;
	}

	bool RenderState::GetDepthMask() const
	{
		return depth_mask;
	}

	GLenum RenderState::GetDepthFunc() const
	{
		return depth_function;
	}

	bool RenderState::GetDepthTest() const
	{
		return depth_test;
	}

	bool RenderState::GetBlend() const
	{
		return blend;
	}

	GLenum RenderState::GetBlendSrc() const
	{
		return blend_src;
	}

	GLenum RenderState::GetBlendDst() const
	{
		return blend_dst;
	}

	bool RenderState::GetCullFace() const
	{
		return cull_face;
	}

	GLenum RenderState::GetCullFaceMode() const
	{
		return cull_face_mode;
	}

	void RenderState::Apply() const
	{
		static RenderState lastState;

		if (!modified)
		{
			return;
		}

		if (static bool firstCall = true; firstCall)
		{
			firstCall = false;

			if (depth_test)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
			glDepthFunc(depth_function);

			if (blend)
			{
				glEnable(GL_BLEND);
				glBlendFunc(blend_src, blend_dst);
			}
			else
			{
				glDisable(GL_BLEND);
			}

			if (cull_face)
			{
				glEnable(GL_CULL_FACE);
				glCullFace(cull_face_mode);
			}
			else
			{
				glDisable(GL_CULL_FACE);
			}

			if (depth_mask)
			{
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDepthMask(GL_FALSE);
			}

			lastState = *this;
			return;
		}

		if (depth_test != lastState.depth_test)
		{
			if (depth_test)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}

		if (depth_function != lastState.depth_function)
		{
			glDepthFunc(depth_function);
		}

		if (blend != lastState.blend)
		{
			if (blend)
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);
		}

		if (blend && (blend_src != lastState.blend_src || blend_dst != lastState.blend_dst))
		{
			glBlendFunc(blend_src, blend_dst);
		}

		if (cull_face != lastState.cull_face)
		{
			if (cull_face)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		if (cull_face && cull_face_mode != lastState.cull_face_mode)
		{
			glCullFace(cull_face_mode);
		}

		if (depth_mask != lastState.depth_mask)
		{
			if (depth_mask)
			{
				glDepthMask(GL_TRUE);
			}
			else
			{
				glDepthMask(GL_FALSE);
			}
		}

		if (shader != lastState.shader && shader.IsValid())
		{
			shader.Get<Shader>()->Bind();
		}

		for (int i = 0; i < bound_textures.size(); i++)
		{
			if (bound_textures[i] != lastState.bound_textures[i] && bound_textures[i].IsValid())
			{
				bound_textures[i].Get<Texture>()->Bind(i);
			}
		}

		if (ssbo_id != 0 && ssbo_id != lastState.ssbo_id && binding_point != 0)
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, ssbo_id);
		}

		if (frame_buffer != lastState.frame_buffer && frame_buffer)
		{
			frame_buffer->Bind();
		}

		lastState = *this;
	}

	void RenderState::Reset()
	{
		SetShader(AssetID::INVALID_ASSET_ID);

		for (int i = 0; i < bound_textures.size(); i++)
		{
			SetTexture(AssetID::INVALID_ASSET_ID, i);
		}

		SetSSBO(0, 0);
		SetDepthMask(true);
		SetDepthFunc(GL_LESS);
		SetDepthTest(true);
		SetBlend(false);
		SetBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		SetCullFace(true);
		SetCullFaceMode(GL_BACK);
		SetFramebuffer(nullptr);
	}
}