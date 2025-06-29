#pragma once

#include <span>
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "array_pool.h"
#include "camera.h"
#include "shader.h"
#include "texture.h"
#include "asset_manager.h"

namespace Eon
{
	class Mesh
	{
	public:
		virtual ~Mesh() = default;

		void Render() const;
		static void RenderMeshes(Camera& camera, glm::vec3 cameraPosition, const std::vector<Mesh*>& meshes, Shader& shader);

		virtual void Setup() = 0;

		bool IsSetup() const;

	protected:
		enum class AttributeLocation : int
		{
			Position = 0,
			TextureCoord = 1,
			Color = 2,
			Normal = 3
		};

		template<typename T>
		void GenVBO(GLuint* vbo, std::vector<T, PoolAllocator<T>>& data)
		{
			glGenBuffers(1, vbo);
			glBindBuffer(GL_ARRAY_BUFFER, *vbo);
			glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);

			data.clear();
			data.shrink_to_fit();
		}

		void SetupVBO(GLuint vbo, int location, int size, int type) const;

		GLsizei CalculateVertexCount(const std::vector<size_t>& sizes) const;

		void EndSetup();

		GLuint vao = 0;
		GLsizei vertex_count = 0;
		bool setup = false;
		TextureID texture_id;
	};

	class PositionTextureColorNormalMesh : public Mesh
	{
	public:
		explicit PositionTextureColorNormalMesh(
			std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& positions,
			std::vector<glm::vec2, PoolAllocator<glm::vec2>>&& texture_coords,
			std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& colors,
			std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& normals,
			TextureID textureID);
		~PositionTextureColorNormalMesh() final;

		void Setup() override;

	private:
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions;
		std::vector<glm::vec2, PoolAllocator<glm::vec2>> texture_coords;
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> colors;
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> normals;
		GLuint position_vbo = 0;
		GLuint texture_coords_vbo = 0;
		GLuint color_vbo = 0;
		GLuint normal_vbo = 0;
	};

	class PositionTextureColorMesh : public Mesh
	{
	public:
		PositionTextureColorMesh();
		~PositionTextureColorMesh();

		void Setup() override;

	private:
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions;
		std::vector<glm::vec2, PoolAllocator<glm::vec2>> texture_coords;
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> colors;
		GLuint position_vbo;
		GLuint texture_coords_vbo;
		GLuint color_vbo;
	};

	class PositionTextureMesh : public Mesh
	{
	public:
		PositionTextureMesh();
		~PositionTextureMesh();

		void Setup() override;

	private:
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions;
		std::vector<glm::vec2, PoolAllocator<glm::vec2>> texture_coords;
		GLuint position_vbo;
		GLuint texture_coords_vbo;
	};

	class PositionColorMesh : public Mesh
	{
	public:
		PositionColorMesh();
		~PositionColorMesh();

		void Setup() override;

	private:
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions;
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> colors;
		GLuint position_vbo;
		GLuint color_vbo;
	};

	class PositionMesh : public Mesh
	{
	public:
		PositionMesh();
		~PositionMesh();

		void Setup() override;

	private:
		std::vector<glm::vec3, PoolAllocator<glm::vec3>> positions;
		GLuint position_vbo;
	};
}