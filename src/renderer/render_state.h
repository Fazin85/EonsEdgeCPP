#pragma once

#include <glad/glad.h>
#include "../asset_manager.h"
#include <array>

namespace Eon
{
	class Framebuffer;

	class RenderState
	{
	public:
		bool SetShader(ShaderID shader);
		bool SetTexture(TextureID texture, int unit = 0);

		Shader* GetShader() const;
		Texture* GetTexture(int unit = 0) const;

		void SetGBufferTextures(Texture* albedo, Texture* normal, Texture* position);
		void SetSSRTexture(Texture* ssrTexture);
		void SetFramebuffer(Framebuffer* frameBuffer);
		void SetSSBO(GLuint ssboId, GLuint bindingPoint);
		void SetDepthMask(bool depthMask);
		void SetDepthFunc(GLenum depthFunction);
		void SetDepthTest(bool depthTest);
		void SetBlend(bool blend);
		void SetBlendFunc(GLenum blendSrc, GLenum blendDst);
		void SetCullFace(bool cullFace);
		void SetCullFaceMode(GLenum cullFaceMode);

		// binds the gBuffer textures on unit 0, 1 and 2
		// use with caution
		void BindGBufferTextures();

		Texture* GetSSRTexture() const;
		Framebuffer* GetFramebuffer() const;
		bool GetDepthMask() const;
		GLenum GetDepthFunc() const;
		bool GetDepthTest() const;
		bool GetBlend() const;
		GLenum GetBlendSrc() const;
		GLenum GetBlendDst() const;
		bool GetCullFace() const;
		GLenum GetCullFaceMode() const;

		void Apply() const;
		void Reset();

	private:
		GLuint ssbo_id = -1;
		GLuint binding_point = -1;
		ShaderID shader = AssetID::INVALID_ASSET_ID;
		Texture* g_buffer_albedo = nullptr;
		Texture* g_buffer_normal = nullptr;
		Texture* g_buffer_position = nullptr;
		Texture* ssr_texture = nullptr;
		Framebuffer* frame_buffer = nullptr;
		std::array<TextureID, 32> bound_textures;
		GLenum depth_function = GL_LESS;
		bool depth_mask = true;
		bool depth_test = true;
		bool blend = false;
		GLenum blend_src = GL_SRC_ALPHA;
		GLenum blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		bool cull_face = true;
		GLenum cull_face_mode = GL_BACK;
		bool modified = true;
	};
}