#pragma once

#include "num.h"
#include "vertex_buffer_object.h"

namespace Eon
{
	class VertexArrayObject
	{
	public:
		void Init();
		void Link(int location, int size, VertexBufferObject& vbo, u32 type,
			bool integer);
		void Bind() const;
		void Unbind();
		void Destroy();
		bool Initialized() const;

	private:
		bool initialized = false;
		u32 id = 0;
	};
}  // namespace Eon