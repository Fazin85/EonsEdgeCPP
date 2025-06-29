#include "Framebuffer.h"
#include "gl_error_check.h"

namespace Eon
{
	Framebuffer::Framebuffer(const FramebufferSpec& spec) : m_spec(spec)
	{
		Create();
	}

	Framebuffer::~Framebuffer()
	{
		Cleanup();
	}

	Framebuffer::Framebuffer(Framebuffer&& other) noexcept
	{
		*this = std::move(other);
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		if (this != &other)
		{
			Cleanup();
			m_framebufferID = other.m_framebufferID;
			m_colorAttachments = std::move(other.m_colorAttachments);
			m_depthAttachment = other.m_depthAttachment;
			m_stencilAttachment = other.m_stencilAttachment;
			m_depthStencilAttachment = other.m_depthStencilAttachment;
			m_spec = other.m_spec;

			other.m_framebufferID = 0;
			other.m_depthAttachment = 0;
			other.m_stencilAttachment = 0;
			other.m_depthStencilAttachment = 0;
		}
		return *this;
	}

	void Framebuffer::Bind() const
	{
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID));
		GL_CHECK(glViewport(0, 0, m_spec.width, m_spec.height));
	}

	void Framebuffer::Unbind() const
	{
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void Framebuffer::Resize(int width, int height)
	{
		m_spec.width = width;
		m_spec.height = height;
		Recreate();
	}

	void Framebuffer::BindColorTexture(int index, int textureSlot) const
	{
		if (index < static_cast<int>(m_colorAttachments.size()))
		{
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureSlot));
			GL_CHECK(glBindTexture(m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
				m_colorAttachments[index]));
		}
	}

	void Framebuffer::BindDepthTexture(int textureSlot) const
	{
		if (m_depthAttachment != 0)
		{
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureSlot));
			GL_CHECK(glBindTexture(m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
				m_depthAttachment));
		}
	}

	void Framebuffer::BindDepthStencilTexture(int textureSlot) const
	{
		if (m_depthStencilAttachment != 0)
		{
			GL_CHECK(glActiveTexture(GL_TEXTURE0 + textureSlot));
			GL_CHECK(glBindTexture(m_spec.samples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D,
				m_depthStencilAttachment));
		}
	}

	void Framebuffer::SetDrawBuffers() const
	{
		if (m_colorAttachments.empty())
		{
			GL_CHECK(glDrawBuffer(GL_NONE));
		}
		else
		{
			std::vector<GLenum> buffers;
			for (size_t i = 0; i < m_colorAttachments.size(); ++i)
			{
				buffers.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i));
			}
			GL_CHECK(glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data()));
		}
	}

	void Framebuffer::Clear(float r, float g, float b, float a, float depth, int stencil) const
	{
		Bind();
		GL_CHECK(glClearColor(r, g, b, a));
		GLbitfield mask = GL_COLOR_BUFFER_BIT;

		if (m_depthAttachment != 0 || m_depthStencilAttachment != 0)
		{
			GL_CHECK(glClearDepth(depth));
			mask |= GL_DEPTH_BUFFER_BIT;
		}

		if (m_stencilAttachment != 0 || m_depthStencilAttachment != 0)
		{
			GL_CHECK(glClearStencil(stencil));
			mask |= GL_STENCIL_BUFFER_BIT;
		}

		GL_CHECK(glClear(mask));
	}

	void Framebuffer::BlitTo(const Framebuffer& target, GLbitfield mask, GLenum filter) const
	{
		GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebufferID));
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target.m_framebufferID));
		GL_CHECK(glBlitFramebuffer(0, 0, m_spec.width, m_spec.height,
			0, 0, target.m_spec.width, target.m_spec.height,
			mask, filter));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void Framebuffer::BlitToScreen(GLbitfield mask, GLenum filter, int screenWidth, int screenHeight) const
	{
		GL_CHECK(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_framebufferID));
		GL_CHECK(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));

		if (screenWidth == 0) screenWidth = m_spec.width;
		if (screenHeight == 0) screenHeight = m_spec.height;

		GL_CHECK(glBlitFramebuffer(0, 0, m_spec.width, m_spec.height,
			0, 0, screenWidth, screenHeight,
			mask, filter));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	bool Framebuffer::IsComplete() const
	{
		Bind();
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		Unbind();
		return status == GL_FRAMEBUFFER_COMPLETE;
	}

	GLuint Framebuffer::GetFramebufferID() const
	{
		return m_framebufferID;
	}

	GLuint Framebuffer::GetColorAttachment(int index) const
	{
		return index < static_cast<int>(m_colorAttachments.size()) ? m_colorAttachments[index] : 0;
	}

	GLuint Framebuffer::GetDepthAttachment() const
	{
		return m_depthAttachment;
	}

	GLuint Framebuffer::GetStencilAttachment() const
	{
		return m_stencilAttachment;
	}

	GLuint Framebuffer::GetDepthStencilAttachment() const
	{
		return m_depthStencilAttachment;
	}

	const Framebuffer::FramebufferSpec& Framebuffer::GetSpec() const
	{
		return m_spec;
	}

	int Framebuffer::GetWidth() const
	{
		return m_spec.width;
	}

	int Framebuffer::GetHeight() const
	{
		return m_spec.height;
	}

	int Framebuffer::GetColorAttachmentCount() const
	{
		return static_cast<int>(m_colorAttachments.size());
	}

	void Framebuffer::Create()
	{
		GL_CHECK(glGenFramebuffers(1, &m_framebufferID));
		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID));

		m_colorAttachments.resize(m_spec.colorAttachments.size());
		for (size_t i = 0; i < m_spec.colorAttachments.size(); ++i)
		{
			CreateColorAttachment(i, m_spec.colorAttachments[i]);
		}

		if (m_spec.hasDepthAttachment)
		{
			CreateDepthAttachment();
		}

		if (m_spec.hasStencilAttachment)
		{
			CreateStencilAttachment();
		}

		if (m_spec.hasDepthStencilAttachment)
		{
			CreateDepthStencilAttachment();
		}

		SetDrawBuffers();

		if (!IsComplete())
		{
			Cleanup();
			throw std::runtime_error("Framebuffer not complete!");
		}

		GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	void Framebuffer::CreateColorAttachment(size_t index, const ColorAttachmentSpec& spec)
	{
		GL_CHECK(glGenTextures(1, &m_colorAttachments[index]));

		GLenum target = (m_spec.samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GL_CHECK(glBindTexture(target, m_colorAttachments[index]));

		if (m_spec.samples > 1)
		{
			GL_CHECK(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_spec.samples,
				spec.internalFormat, m_spec.width, m_spec.height, GL_TRUE));
		}
		else
		{
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, spec.internalFormat, m_spec.width, m_spec.height,
				0, spec.format, spec.type, nullptr));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, spec.minFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, spec.magFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, spec.wrapS));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, spec.wrapT));
		}

		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(index),
			target, m_colorAttachments[index], 0));
	}

	void Framebuffer::CreateDepthAttachment()
	{
		GL_CHECK(glGenTextures(1, &m_depthAttachment));

		GLenum target = (m_spec.samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GL_CHECK(glBindTexture(target, m_depthAttachment));

		if (m_spec.samples > 1)
		{
			GL_CHECK(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_spec.samples,
				m_spec.depthSpec.internalFormat, m_spec.width, m_spec.height, GL_TRUE));
		}
		else
		{
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, m_spec.depthSpec.internalFormat,
				m_spec.width, m_spec.height, 0, m_spec.depthSpec.format,
				m_spec.depthSpec.type, nullptr));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_spec.depthSpec.minFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_spec.depthSpec.magFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_spec.depthSpec.wrapS));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_spec.depthSpec.wrapT));
		}

		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, m_depthAttachment, 0));
	}

	void Framebuffer::CreateStencilAttachment()
	{
		GL_CHECK(glGenTextures(1, &m_stencilAttachment));

		GLenum target = (m_spec.samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GL_CHECK(glBindTexture(target, m_stencilAttachment));

		if (m_spec.samples > 1)
		{
			GL_CHECK(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_spec.samples,
				m_spec.stencilSpec.internalFormat, m_spec.width, m_spec.height, GL_TRUE));
		}
		else
		{
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, m_spec.stencilSpec.internalFormat,
				m_spec.width, m_spec.height, 0, m_spec.stencilSpec.format,
				m_spec.stencilSpec.type, nullptr));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_spec.stencilSpec.minFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_spec.stencilSpec.magFilter));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_spec.stencilSpec.wrapS));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_spec.stencilSpec.wrapT));
		}

		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, target, m_stencilAttachment, 0));
	}

	void Framebuffer::CreateDepthStencilAttachment()
	{
		GL_CHECK(glGenTextures(1, &m_depthStencilAttachment));

		GLenum target = (m_spec.samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		GL_CHECK(glBindTexture(target, m_depthStencilAttachment));

		if (m_spec.samples > 1)
		{
			GL_CHECK(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_spec.samples,
				GL_DEPTH24_STENCIL8, m_spec.width, m_spec.height, GL_TRUE));
		}
		else
		{
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_spec.width, m_spec.height,
				0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target,
			m_depthStencilAttachment, 0);
	}

	void Framebuffer::Recreate()
	{
		Cleanup();
		Create();
	}

	void Framebuffer::Cleanup()
	{
		if (m_framebufferID != 0)
		{
			GL_CHECK(glDeleteFramebuffers(1, &m_framebufferID));
			m_framebufferID = 0;
		}

		if (!m_colorAttachments.empty())
		{
			GL_CHECK(glDeleteTextures(static_cast<GLsizei>(m_colorAttachments.size()), m_colorAttachments.data()));
			m_colorAttachments.clear();
		}

		if (m_depthAttachment != 0)
		{
			GL_CHECK(glDeleteTextures(1, &m_depthAttachment));
			m_depthAttachment = 0;
		}

		if (m_stencilAttachment != 0)
		{
			GL_CHECK(glDeleteTextures(1, &m_stencilAttachment));
			m_stencilAttachment = 0;
		}

		if (m_depthStencilAttachment != 0)
		{
			GL_CHECK(glDeleteTextures(1, &m_depthStencilAttachment));
			m_depthStencilAttachment = 0;
		}
	}
}