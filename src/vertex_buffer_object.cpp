#include "vertex_buffer_object.h"

#include <glad/glad.h>

namespace Eon
{
	void VertexBufferObject::Init(std::vector<u32>& data)
	{
		glGenBuffers(1, &id);

		glBindBuffer(GL_ARRAY_BUFFER, id);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(u32), data.data(),
			GL_STATIC_DRAW);
	}

	void VertexBufferObject::Bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); }

	void VertexBufferObject::Unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }

	void VertexBufferObject::Destroy() const { glDeleteBuffers(1, &id); }
}  // namespace Eon
