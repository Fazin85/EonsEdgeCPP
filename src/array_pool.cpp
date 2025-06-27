#include "array_pool.h"

namespace Eon
{
	PoolAllocators& PoolAllocators::GetInstance()
	{
		static PoolAllocators instance;
		if (!instance.initialized)
		{
			instance.u32_pool = new ArrayPool<uint32_t>();
			instance.u8_pool = new ArrayPool<uint8_t>();
			instance.ivec2_pool = new ArrayPool<glm::ivec2>();
			instance.ivec3_pool = new ArrayPool<glm::ivec3>();
			instance.vec2_pool = new ArrayPool<glm::vec2>();
			instance.vec3_pool = new ArrayPool<glm::vec3>();

			instance.u32_allocator = new PoolAllocator(instance.u32_pool);
			instance.u8_allocator = new PoolAllocator(instance.u8_pool);
			instance.ivec2_allocator = new PoolAllocator(instance.ivec2_pool);
			instance.ivec3_allocator = new PoolAllocator(instance.ivec3_pool);
			instance.vec2_allocator = new PoolAllocator(instance.vec2_pool);
			instance.vec3_allocator = new PoolAllocator(instance.vec3_pool);
			instance.initialized = true;
		}

		return instance;
	}
}
