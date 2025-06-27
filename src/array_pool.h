#pragma once

#include <map>
#include <memory>
#include <vector>

namespace Eon
{
	template<typename T>
	class ArrayPool
	{
	public:
		std::shared_ptr<T[]> Allocate(size_t count)
		{
			if (auto it = pool.lower_bound(count); it != pool.end())
			{
				auto result = it->second;
				pool.erase(it);
				return result;
			}
			return std::make_shared<T[]>(count);
		}

		void Deallocate(std::shared_ptr<T[]> ptr, size_t count)
		{
			pool[count] = ptr;
		}

		size_t PooledCount() const
		{
			return pool.size();
		}

	private:
		std::map<size_t, std::shared_ptr<T[]>> pool;
	};

	template<typename T>
	class PoolAllocator
	{
	public:
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

		template<typename U>
		struct rebind { using other = PoolAllocator<U>; };

		PoolAllocator() = default;
		explicit PoolAllocator(ArrayPool<T>* pool) : pool(pool) {}

		template<typename U>
		explicit PoolAllocator(const PoolAllocator<U>& other) : pool(other.pool) {}

		pointer allocate(size_type n)
		{
			if (pool)
			{
				auto shared_ptr = pool->Allocate(n);
				allocated_blocks[shared_ptr.get()] = shared_ptr;
				return shared_ptr.get();
			}
			return static_cast<pointer>(::operator new(n * sizeof(T)));
		}

		void deallocate(pointer p, size_type n)
		{
			if (pool)
			{
				auto it = allocated_blocks.find(p);
				if (it != allocated_blocks.end())
				{
					pool->Deallocate(it->second, n);
					allocated_blocks.erase(it);
					return;
				}
			}
			::operator delete(p);
		}

		/**
		* Returns the number of pooled elements in the array pool.
		*
		* @return The number of pooled elements.
		*/
		size_t PooledCount() const
		{
			if (pool)
			{
				return pool->PooledCount();
			}

			return 0;
		}

		template<typename U>
		bool operator==(const PoolAllocator<U>& other) const
		{
			return pool == other.pool;
		}

		template<typename U>
		bool operator!=(const PoolAllocator<U>& other) const
		{
			return !(*this == other);
		}

	private:
		ArrayPool<T>* pool = nullptr;
		std::map<T*, std::shared_ptr<T[]>> allocated_blocks;

		template<typename U> friend class PoolAllocator;
	};

	class PoolAllocators
	{
	private:
		static inline ArrayPool<unsigned int> u32_pool;
		static inline ArrayPool<unsigned char> u8_pool;
		static inline ArrayPool<glm::ivec2> ivec2_pool;
		static inline ArrayPool<glm::ivec3> ivec3_pool;
		static inline ArrayPool<glm::vec2> vec2_pool;
		static inline ArrayPool<glm::vec3> vec3_pool;

	public:
		//ONLY USE ON MAIN THREAD
		static inline PoolAllocator<unsigned int> u32_allocator{ &u32_pool };
		//ONLY USE ON MAIN THREAD
		static inline PoolAllocator<unsigned char> u8_allocator{ &u8_pool };
		//ONLY USE ON MAIN THREAD
		static inline PoolAllocator<glm::ivec2> ivec2_allocator{ &ivec2_pool };
		//ONLY USE ON MAIN THREAD
		static inline PoolAllocator<glm::ivec3> ivec3_allocator{ &ivec3_pool };
		//ONLY USE ON MAIN THREAD
		static inline PoolAllocator<glm::vec2> vec2_allocator{ &vec2_pool };
		//ONLY USE ON MAIN THREAD
		static inline PoolAllocator<glm::vec3> vec3_allocator{ &vec3_pool };
	};
}