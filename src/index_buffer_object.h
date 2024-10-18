#pragma once

#include <vector>

#include "num.h"

namespace Eon
{
	class IndexBufferObject
	{
	public:
		void Init(u32* data, size_t dataSize);
		void Bind() const;
		void Unbind();
		void Destroy() const;

	private:
		u32 id;
	};
}  // namespace Eon