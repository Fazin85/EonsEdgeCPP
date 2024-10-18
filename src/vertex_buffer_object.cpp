#include "vertex_buffer_object.h"

#include <glad/glad.h>

namespace Eon
{
	void VertexBufferObject::Init(u32* data, size_t dataSize)
	{
		glGenBuffers(1, &id);

		Bind();
		glBufferData(GL_ARRAY_BUFFER, dataSize * sizeof(u32), data,
			GL_STATIC_DRAW);
	}

	void VertexBufferObject::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); }

	void VertexBufferObject::Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	void VertexBufferObject::Destroy() const { glDeleteBuffers(1, &id); }
}  // namespace Eon
