#pragma once

#include <map>
#include <unordered_map>
#include <memory>
#include <vector>
#include "glm/glm.hpp"

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

	// Shared state for tracking allocated blocks
	template<typename T>
	struct AllocatorState
	{
		std::unordered_map<T*, std::shared_ptr<T[]>> allocated_blocks;
		ArrayPool<T>* pool = nullptr;

		AllocatorState() = default;
		explicit AllocatorState(ArrayPool<T>* p) : pool(p) {}
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

		// Required for C++17 and later
		using propagate_on_container_copy_assignment = std::false_type;
		using propagate_on_container_move_assignment = std::false_type;
		using propagate_on_container_swap = std::false_type;
		using is_always_equal = std::false_type;

		template<typename U>
		struct rebind { using other = PoolAllocator<U>; };

		PoolAllocator() : state(std::make_shared<AllocatorState<T>>()) {}

		explicit PoolAllocator(ArrayPool<T>* pool)
			: state(std::make_shared<AllocatorState<T>>(pool))
		{
		}

		// Copy constructor
		PoolAllocator(const PoolAllocator& other) : state(other.state) {}

		// Rebind constructor
		template<typename U>
		explicit PoolAllocator(const PoolAllocator<U>& other)
			: state(std::make_shared<AllocatorState<T>>())
		{
			// Note: We can't directly copy the pool pointer due to type differences
			// This is intentional - each type needs its own pool
		}

		// Assignment operator
		PoolAllocator& operator=(const PoolAllocator& other)
		{
			if (this != &other)
			{
				state = other.state;
			}
			return *this;
		}

		pointer allocate(size_type n)
		{
			if (state->pool)
			{
				auto shared_ptr = state->pool->Allocate(n);
				state->allocated_blocks[shared_ptr.get()] = shared_ptr;
				return shared_ptr.get();
			}
			return static_cast<pointer>(::operator new(n * sizeof(T)));
		}

		void deallocate(pointer p, size_type n)
		{
			if (state->pool)
			{
				auto it = state->allocated_blocks.find(p);
				if (it != state->allocated_blocks.end())
				{
					state->pool->Deallocate(it->second, n);
					state->allocated_blocks.erase(it);
					return;
				}
			}
			::operator delete(p);
		}

		// Required for STL containers - construct objects
		template<typename U, typename... Args>
		void construct(U* p, Args&&... args) const
		{
			::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
		}

		// Required for STL containers - destroy objects
		template<typename U>
		void destroy(U* p) const
		{
			p->~U();
		}

		// Returns the maximum number of elements that can be allocated
		size_type max_size() const noexcept
		{
			return std::numeric_limits<size_type>::max() / sizeof(T);
		}

		// Returns the address of an object
		pointer address(reference x) const noexcept
		{
			return std::addressof(x);
		}

		const_pointer address(const_reference x) const noexcept
		{
			return std::addressof(x);
		}

		/**
		 * Returns the number of pooled elements in the array pool.
		 *
		 * @return The number of pooled elements.
		 */
		size_t PooledCount() const
		{
			if (state->pool)
			{
				return state->pool->PooledCount();
			}
			return 0;
		}

		template<typename U>
		bool operator==(const PoolAllocator<U>& other) const
		{
			return state->pool == other.state->pool;
		}

		template<typename U>
		bool operator!=(const PoolAllocator<U>& other) const
		{
			return !(*this == other);
		}

	private:
		std::shared_ptr<AllocatorState<T>> state;

		template<typename U> friend class PoolAllocator;
	};

	class PoolAllocators
	{
	private:
		ArrayPool<unsigned int>* u32_pool = nullptr;
		ArrayPool<unsigned char>* u8_pool = nullptr;
		ArrayPool<glm::ivec2>* ivec2_pool = nullptr;
		ArrayPool<glm::ivec3>* ivec3_pool = nullptr;
		ArrayPool<glm::vec2>* vec2_pool = nullptr;
		ArrayPool<glm::vec3>* vec3_pool = nullptr;
		bool initialized = false;

	public:
		//ONLY USE ON MAIN THREAD
		static PoolAllocators& GetInstance();

		//ONLY USE ON MAIN THREAD
		PoolAllocator<unsigned int>* u32_allocator = nullptr;
		//ONLY USE ON MAIN THREAD
		PoolAllocator<unsigned char>* u8_allocator = nullptr;
		//ONLY USE ON MAIN THREAD
		PoolAllocator<glm::ivec2>* ivec2_allocator = nullptr;
		//ONLY USE ON MAIN THREAD
		PoolAllocator<glm::ivec3>* ivec3_allocator = nullptr;
		//ONLY USE ON MAIN THREAD
		PoolAllocator<glm::vec2>* vec2_allocator = nullptr;
		//ONLY USE ON MAIN THREAD
		PoolAllocator<glm::vec3>* vec3_allocator = nullptr;
	};
}