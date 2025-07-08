#include <iostream>
#include <fstream>
#include <random>
#include "default_render_pipeline.h"
#include "opaque_render_pass.h"
#include "translucent_render_pass.h"
#include "ssr_render_pass.h"
#include "../../framebuffer.h"

namespace Eon
{
	static GLuint loadTexture3D(const std::string& filepath,
		int width, int height, int depth,
		GLenum internalFormat = GL_RG16F,
		GLenum format = GL_RG,
		GLenum type = GL_HALF_FLOAT)
	{

		// Calculate expected file size
		size_t components = 0;
		switch (format)
		{
		case GL_RED: components = 1; break;
		case GL_RG: components = 2; break;
		case GL_RGB: components = 3; break;
		case GL_RGBA: components = 4; break;
		default: components = 2; break;
		}

		size_t typeSize = 0;
		switch (type)
		{
		case GL_UNSIGNED_BYTE:
		case GL_BYTE: typeSize = 1; break;
		case GL_UNSIGNED_SHORT:
		case GL_SHORT:
		case GL_HALF_FLOAT: typeSize = 2; break;
		case GL_FLOAT:
		case GL_UNSIGNED_INT:
		case GL_INT: typeSize = 4; break;
		default: typeSize = 2; break;
		}

		size_t expectedSize = width * height * depth * components * typeSize;

		// Open and read the .dat file
		std::ifstream file(filepath, std::ios::binary);
		if (!file.is_open())
		{
			std::cerr << "Error: Could not open file " << filepath << std::endl;
			return 0;
		}

		// Get file size
		file.seekg(0, std::ios::end);
		size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		if (fileSize != expectedSize)
		{
			std::cerr << "Warning: File size (" << fileSize << ") doesn't match expected size (" << expectedSize << ")" << std::endl;
		}

		// Read the data
		std::vector<unsigned char> data(fileSize);
		file.read(reinterpret_cast<char*>(data.data()), fileSize);
		file.close();

		if (!file.good() && !file.eof())
		{
			std::cerr << "Error: Failed to read file " << filepath << std::endl;
			return 0;
		}

		// Generate OpenGL texture
		GLuint textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_3D, textureID);

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		// Upload texture data
		glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, type, data.data());

		// Check for OpenGL errors
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			std::cerr << "OpenGL Error after texture upload: " << error << std::endl;
			glDeleteTextures(1, &textureID);
			return 0;
		}

		glBindTexture(GL_TEXTURE_3D, 0);

		std::cout << "Successfully loaded 3D texture: " << filepath << " (" << width << "x" << height << "x" << depth << ")" << std::endl;
		return textureID;
	}

	static std::unique_ptr<Texture> CreateNoiseTex()
	{
		sf::Image img;
		img.create(256, 256, sf::Color::Black);

		std::random_device rd;
		std::mt19937 gen(rd());

		// For nextInt() - random int in full range
		std::uniform_int_distribution dis(std::numeric_limits<int>::min(),
			std::numeric_limits<int>::max());

		for (int x = 0; x < 256; x++)
		{
			for (int y = 0; y < 256; y++)
			{
				int color = dis(gen) | (255 << 24);
				img.setPixel(x, y, sf::Color(color));
			}
		}

		img.saveToFile("noise.png");
		return std::make_unique<Texture>(img, false);
	}

	DefaultRenderPipeline::DefaultRenderPipeline()
	{
		Framebuffer::FramebufferSpec spec;
		spec.width = Window::Get().getSize().x;
		spec.height = Window::Get().getSize().y;

		spec.colorAttachments = {
			// Albedo
			{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			// Normal 
			{ GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
			// Position + Depth
			{ GL_RGBA32F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
		};

		// Add depth buffer
		spec.hasDepthAttachment = true;

		g_buffer = std::make_unique<Framebuffer>(spec);

		//Framebuffer::FramebufferSpec ssrSpec;
		//ssrSpec.width = Window::Get().getSize().x;
		//ssrSpec.height = Window::Get().getSize().y;

		//ssrSpec.colorAttachments = {
		//	// Albedo
		//	{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE },
		//};

		//ssr_buffer = std::make_unique<Framebuffer>(ssrSpec);

		render_passes.emplace_back(std::make_unique<OpaqueRenderPass>(*this, *g_buffer));
		render_passes.emplace_back(std::make_unique<TranslucentRenderPass>(*this, *g_buffer));

		GLuint skytex = loadTexture3D("content/images/clouds_3d.dat", 256, 256, 64);
		glActiveTexture(GL_TEXTURE15);
		glBindTexture(GL_TEXTURE_3D, skytex);
		//render_passes.emplace_back(std::make_unique<SSRRenderPass>(*this, *ssr_buffer));

		std::unique_ptr<Texture> noisetex = CreateNoiseTex();
		noisetex->Bind(14);
	}

	void DefaultRenderPipeline::BeginFrame()
	{
		render_stats.Reset();
	}

	void DefaultRenderPipeline::Clear()
	{
		render_state.SetFramebuffer(g_buffer.get());
		render_state.Apply();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void DefaultRenderPipeline::Submit(RenderCommandVariant& renderCommand)
	{
		const auto& command = GetRenderCommand(renderCommand);

		switch (command.GetMaterial().GetTransparencyType())
		{
		case TransparencyType::Opaque:
			render_passes[0]->Submit(renderCommand);
			break;
		case TransparencyType::Cutout:
			break;
		case TransparencyType::Transparent:
			render_passes[1]->Submit(renderCommand);
			break;
		}
	}

	void DefaultRenderPipeline::EndFrame()
	{
		for (const auto& pass : render_passes)
		{
			pass->Begin(render_state);
			pass->Execute(render_state);
			pass->End(render_state);
		}

		global_mat4_uniforms.clear();
		global_vec3_uniforms.clear();
		global_float_uniforms.clear();
		global_int_uniforms.clear();
	}

	void DefaultRenderPipeline::Display()
	{
		g_buffer->BlitToScreen();
		//ssr_buffer->BlitToScreen();
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, const glm::mat4& value)
	{
		global_mat4_uniforms[name] = value;
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, const glm::vec3& value)
	{
		global_vec3_uniforms[name] = value;
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, float value)
	{
		global_float_uniforms[name] = value;
	}

	void DefaultRenderPipeline::SetGlobalUniform(const std::string& name, int value)
	{
		global_int_uniforms[name] = value;
	}

	void DefaultRenderPipeline::OnShaderBound(Shader& shader)
	{
		ApplyGlobalUniforms(shader);
		render_stats.shader_binds++;
	}

	void DefaultRenderPipeline::OnTextureBound(Texture& texture)
	{
		render_stats.texture_binds++;
	}

	void DefaultRenderPipeline::ApplyGlobalUniforms(Shader& shader) const
	{
		for (const auto& [name, value] : global_mat4_uniforms)
		{
			shader.UniformMatrix4(name, value);
		}

		for (const auto& [name, value] : global_vec3_uniforms)
		{
			shader.UniformFVec3(name, value);
		}

		for (const auto& [name, value] : global_float_uniforms)
		{
			shader.UniformFloat(name, value);
		}

		for (const auto& [name, value] : global_int_uniforms)
		{
			shader.UniformInt1(name, value);
		}
	}

	RenderPipeline::RenderStats& DefaultRenderPipeline::GetRenderStats()
	{
		return render_stats;
	}
}
