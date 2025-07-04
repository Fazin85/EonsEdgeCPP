#pragma once

#include <glad/glad.h>
#include <vector>
#include <cstddef>

namespace Eon
{
	class SSBO
	{
	private:
		GLuint buffer_id;
		GLuint binding_point;
		size_t buffer_size;
		GLenum usage;

	public:
		SSBO();
		~SSBO();

		SSBO(const SSBO&) = delete;
		SSBO& operator=(const SSBO&) = delete;

		SSBO(SSBO&& other) noexcept;
		SSBO& operator=(SSBO&& other) noexcept;

		void Create(size_t size, GLenum usage = GL_DYNAMIC_DRAW);
		void Bind(GLuint bindingPoint);
		void Unbind() const;

		template<typename T>
		void UploadData(const std::vector<T>& data, size_t offset = 0);

		template<typename T>
		void UploadData(const T* data, size_t count, size_t offset = 0);

		void UploadRaw(const void* data, size_t size, size_t offset = 0) const;

		template<typename T>
		std::vector<T> DownloadData(size_t count, size_t offset = 0);

		void* MapBuffer(GLenum access = GL_READ_WRITE) const;
		void UnmapBuffer() const;

		void Resize(size_t newSize);
		void Clear();

		GLuint GetId() const { return buffer_id; }
		GLuint GetBindingPoint() const { return binding_point; }
		size_t GetSize() const { return buffer_size; }
		bool IsValid() const { return buffer_id != 0; }
	};

	template<typename T>
	void SSBO::UploadData(const std::vector<T>& data, size_t offset)
	{
		UploadRaw(data.data(), data.size() * sizeof(T), offset * sizeof(T));
	}

	template<typename T>
	void SSBO::UploadData(const T* data, size_t count, size_t offset)
	{
		UploadRaw(data, count * sizeof(T), offset * sizeof(T));
	}

	template<typename T>
	std::vector<T> SSBO::DownloadData(size_t count, size_t offset)
	{
		std::vector<T> result(count);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer_id);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset * sizeof(T), count * sizeof(T), result.data());
		return result;
	}
}