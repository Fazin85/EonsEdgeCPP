#include "mesh.h"
#include "log.h"

#include <stdexcept>

namespace Eon
{
	PositionTextureColorNormalMesh::PositionTextureColorNormalMesh(
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& positions,
		std::vector<glm::vec2, PoolAllocator<glm::vec2>>&& texture_coords,
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& colors,
		std::vector<glm::vec3, PoolAllocator<glm::vec3>>&& normals,
		Texture* texture) :
		positions(std::move(positions)),
		texture_coords(std::move(texture_coords)),
		colors(std::move(colors)),
		normals(std::move(normals))
	{
		std::vector<size_t> sizes = { this->positions.size(), this->texture_coords.size(), this->colors.size(), this->normals.size() };
		vertex_count = CalculateVertexCount(sizes);

		this->texture = texture;
	}

	PositionTextureColorNormalMesh::~PositionTextureColorNormalMesh()
	{
		if (!IsSetup())
		{
			return;
		}

		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &position_vbo);
		glDeleteBuffers(1, &texture_coords_vbo);
		glDeleteBuffers(1, &color_vbo);
		glDeleteBuffers(1, &normal_vbo);
	}

	void PositionTextureColorNormalMesh::Setup()
	{
		if (IsSetup())
		{
			EON_WARN("Tried to setup mesh multiple times");
			return;
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		GenVBO(&position_vbo, positions);
		GenVBO(&texture_coords_vbo, texture_coords);
		GenVBO(&color_vbo, colors);
		GenVBO(&normal_vbo, normals);

		BeginVAOSetup();

		{
			using enum AttributeLocation;
			SetupVBO(position_vbo, static_cast<int>(Position), 3, GL_FLOAT);
			SetupVBO(texture_coords_vbo, static_cast<int>(TextureCoord), 2, GL_FLOAT);
			SetupVBO(color_vbo, static_cast<int>(Color), 3, GL_FLOAT);
			SetupVBO(normal_vbo, static_cast<int>(Normal), 3, GL_FLOAT);
		}

		EndSetup();
	}

	void Mesh::Render() const
	{
		if (!IsSetup())
		{
			return;
		}

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertex_count);
	}

	void Mesh::RenderMeshes(Camera& camera, glm::vec3 cameraPosition, const std::vector<Mesh*>& meshes, Shader& shader)
	{
		camera.CalculateViewMatrix(cameraPosition);

		shader.Bind();
		shader.UniformMatrix4NoTranspose("view", camera.ViewMatrix());
		shader.UniformMatrix4NoTranspose("projection", camera.ProjectionMatrix());

		//TODO: sort the meshes based on their distance from the camera and their texture

		const Texture* texture = nullptr;
		for (const Mesh* mesh : meshes)
		{
			if (mesh)
			{
				if (texture != mesh->texture)
				{
					texture = mesh->texture;
					if (texture)
					{
						texture->Bind();
					}
				}

				mesh->Render();
			}
		}
	}

	bool Mesh::IsSetup() const
	{
		return setup;
	}

	void Mesh::BeginVAOSetup() const
	{
		glBindVertexArray(vao);
	}

	void Mesh::SetupVBO(GLuint vbo, int location, int size, int type) const
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glVertexAttribPointer(location, size, type, false, 0, nullptr);
		glEnableVertexAttribArray(location);
	}

	GLsizei Mesh::CalculateVertexCount(const std::vector<size_t>& sizes) const
	{
		if (sizes.empty())
		{
			throw std::runtime_error("Vertex data is empty!");
		}

		for (size_t i = 0; i < sizes.size(); i++)
		{
			if (i > 0 && sizes[i] != sizes[i - 1])
			{
				throw std::runtime_error("Vertex data is not correct!");
			}
		}

		return static_cast<GLsizei>(sizes[0]);
	}

	void Mesh::EndSetup()
	{
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		setup = true;
	}
}
