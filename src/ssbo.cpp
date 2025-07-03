#include "ssbo.h"
#include <iostream>
#include <algorithm>

namespace Eon
{
	SSBO::SSBO() : buffer_id(0), binding_point(0), buffer_size(0), usage(GL_DYNAMIC_DRAW) {}

	SSBO::~SSBO()
	{
		Clear();
	}

	SSBO::SSBO(SSBO&& other) noexcept
		: buffer_id(other.buffer_id), binding_point(other.binding_point),
		buffer_size(other.buffer_size), usage(other.usage)
	{
		other.buffer_id = 0;
		other.binding_point = 0;
		other.buffer_size = 0;
	}

	SSBO& SSBO::operator=(SSBO&& other) noexcept
	{
		if (this != &other)
		{
			Clear();
			buffer_id = other.buffer_id;
			binding_point = other.binding_point;
			buffer_size = other.buffer_size;
			usage = other.usage;

			other.buffer_id = 0;
			other.binding_point = 0;
			other.buffer_size = 0;
		}
		return *this;
	}

	void SSBO::Create(size_t size, GLenum usageHint)
	{
		if (buffer_id != 0)
		{
			Clear();
		}

		glGenBuffers(1, &buffer_id);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, usageHint);

		buffer_size = size;
		usage = usageHint;
	}

	void SSBO::Bind(GLuint bindingPointIndex)
	{
		if (buffer_id == 0) return;

		binding_point = bindingPointIndex;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, buffer_id);
	}

	void SSBO::Unbind() const
	{
		if (binding_point != 0)
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, 0);
		}
	}

	void SSBO::UploadRaw(const void* data, size_t size, size_t offset) const
	{
		if (buffer_id == 0 || offset + size > buffer_size) return;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
	}

	void* SSBO::MapBuffer(GLenum access) const
	{
		if (buffer_id == 0) return nullptr;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
		return glMapBuffer(GL_SHADER_STORAGE_BUFFER, access);
	}

	void SSBO::UnmapBuffer() const
	{
		if (buffer_id == 0) return;

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}

	void SSBO::Resize(size_t newSize)
	{
		if (buffer_id == 0)
		{
			Create(newSize, usage);
			return;
		}

		void* old_data = nullptr;
		size_t copy_size = std::min(buffer_size, newSize);

		if (copy_size > 0)
		{
			old_data = MapBuffer(GL_READ_ONLY);
			if (old_data)
			{
				void* temp_data = malloc(copy_size);
				memcpy(temp_data, old_data, copy_size);
				UnmapBuffer();
				old_data = temp_data;
			}
		}

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
		glBufferData(GL_SHADER_STORAGE_BUFFER, newSize, old_data, usage);

		if (old_data && copy_size > 0)
		{
			free(old_data);
		}

		buffer_size = newSize;
	}

	void SSBO::Clear()
	{
		if (buffer_id != 0)
		{
			glDeleteBuffers(1, &buffer_id);
			buffer_id = 0;
			binding_point = 0;
			buffer_size = 0;
		}
	}
}