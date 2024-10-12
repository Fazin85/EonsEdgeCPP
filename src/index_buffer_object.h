#pragma once

#include <vector>

#include "num.h"

namespace Eon
{
	class IndexBufferObject
	{
	public:
		void Init(std::vector<u32>& data);
		void Bind() const;
		void Unbind();
		void Destroy() const;

	private:
		u32 id;
	};
}  // namespace Eon