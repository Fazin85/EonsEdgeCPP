#pragma once

#include <glad/glad.h>
#include <vector>
#include <stdexcept>

namespace Eon
{
	class Framebuffer
	{
	public:
		struct ColorAttachmentSpec
		{
			GLenum internalFormat = GL_RGBA8;
			GLenum format = GL_RGBA;
			GLenum type = GL_UNSIGNED_BYTE;
			GLenum minFilter = GL_LINEAR;
			GLenum magFilter = GL_LINEAR;
			GLenum wrapS = GL_CLAMP_TO_EDGE;
			GLenum wrapT = GL_CLAMP_TO_EDGE;
		};

		struct DepthAttachmentSpec
		{
			GLenum internalFormat = GL_DEPTH_COMPONENT24;
			GLenum format = GL_DEPTH_COMPONENT;
			GLenum type = GL_FLOAT;
			GLenum minFilter = GL_LINEAR;
			GLenum magFilter = GL_LINEAR;
			GLenum wrapS = GL_CLAMP_TO_EDGE;
			GLenum wrapT = GL_CLAMP_TO_EDGE;
		};

		struct StencilAttachmentSpec
		{
			GLenum internalFormat = GL_STENCIL_INDEX8;
			GLenum format = GL_STENCIL_INDEX;
			GLenum type = GL_UNSIGNED_BYTE;
			GLenum minFilter = GL_NEAREST;
			GLenum magFilter = GL_NEAREST;
			GLenum wrapS = GL_CLAMP_TO_EDGE;
			GLenum wrapT = GL_CLAMP_TO_EDGE;
		};

		struct FramebufferSpec
		{
			int width = 1280;
			int height = 720;
			std::vector<ColorAttachmentSpec> colorAttachments;
			bool hasDepthAttachment = false;
			DepthAttachmentSpec depthSpec;
			bool hasStencilAttachment = false;
			StencilAttachmentSpec stencilSpec;
			bool hasDepthStencilAttachment = false;  // Combined depth-stencil
			int samples = 1;  // For multisampling (1 = no multisampling)
		};

	private:
		GLuint m_framebufferID = 0;
		std::vector<GLuint> m_colorAttachments;
		GLuint m_depthAttachment = 0;
		GLuint m_stencilAttachment = 0;
		GLuint m_depthStencilAttachment = 0;
		FramebufferSpec m_spec;

	public:
		explicit Framebuffer(const FramebufferSpec& spec);
		~Framebuffer();

		Framebuffer(Framebuffer&& other) noexcept;
		Framebuffer& operator=(Framebuffer&& other) noexcept;

		Framebuffer(const Framebuffer&) = delete;
		Framebuffer& operator=(const Framebuffer&) = delete;

		void Bind() const;
		void Unbind() const;
		void Resize(int width, int height);
		void BindColorTexture(int index = 0, int textureSlot = 0) const;
		void BindDepthTexture(int textureSlot = 0) const;
		void BindDepthStencilTexture(int textureSlot = 0) const;
		void SetDrawBuffers() const;
		void Clear(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f,
			float depth = 1.0f, int stencil = 0) const;
		void BlitTo(const Framebuffer& target, GLbitfield mask = GL_COLOR_BUFFER_BIT,
			GLenum filter = GL_LINEAR) const;
		void BlitToScreen(GLbitfield mask = GL_COLOR_BUFFER_BIT,
			GLenum filter = GL_LINEAR,
			int screenWidth = 0, int screenHeight = 0) const;
		bool IsComplete() const;

		GLuint GetFramebufferID() const;
		GLuint GetColorAttachment(int index = 0) const;
		GLuint GetDepthAttachment() const;
		GLuint GetStencilAttachment() const;
		GLuint GetDepthStencilAttachment() const;
		const FramebufferSpec& GetSpec() const;
		int GetWidth() const;
		int GetHeight() const;
		int GetColorAttachmentCount() const;

	private:
		void Create();
		void CreateColorAttachment(size_t index, const ColorAttachmentSpec& spec);
		void CreateDepthAttachment();
		void CreateStencilAttachment();
		void CreateDepthStencilAttachment();
		void Recreate();
		void Cleanup();
	};
}