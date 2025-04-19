#include "chunk_renderer.h"

#include <glad/glad.h>

#include "log.h"
#include <array>
#include <cmath>

namespace Eon
{
	ChunkRenderer::ChunkRenderer(ChunkMeshConstructionData& meshData) : vertices(), indices(), setup(false), list(0)
	{
		indices = meshData.indices;

		vertices = meshData.vertexPositions;
	}

	ChunkRenderer::~ChunkRenderer()
	{
		Destroy();
	}

	void ChunkRenderer::Render() const
	{
		if (!setup)
		{
			return;
		}

		glCallList(list);
	}

	void ChunkRenderer::Setup()
	{
		if (setup)
		{
			EON_WARN("Tried to setup chunk renderer multiple times");
			return;
		}

		list = glGenLists(1);

		glNewList(list, GL_COMPILE); {
			glColor3f(1.0f, 1.0f, 0.0f);

			glBegin(GL_TRIANGLES); {
				for (unsigned int index : indices) {
					glm::ivec3 vertex = vertices[index];
					glVertex3f(vertex.x, vertex.y, vertex.z);
				}
			}
			glEnd();
		}
		glEndList();

		std::vector<glm::ivec3>().swap(vertices);
		std::vector<unsigned int>().swap(indices);

		setup = true;
	}

	void ChunkRenderer::Destroy() const
	{
		if (setup)
		{
			glDeleteLists(list, 1);
		}
	}
}  // namespace Eon
