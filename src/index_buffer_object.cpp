#include "index_buffer_object.h"

#include <glad/glad.h>

namespace Eon {
	void IndexBufferObject::Init(std::vector<u32>& data) {
		glGenBuffers(1, &id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(u32), data.data(),
			GL_STATIC_DRAW);
	}

	void IndexBufferObject::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	}

	void IndexBufferObject::Unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

	void IndexBufferObject::Destroy() const { glDeleteBuffers(1, &id); }
}  // namespace Eon
