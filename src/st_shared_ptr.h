#pragma once

namespace Eon
{
	//Non atomic version of shared_ptr
	template<typename T>
	class st_shared_ptr
	{
	public:
		st_shared_ptr() : ptr(nullptr), ref_count(nullptr) {}

		st_shared_ptr(std::nullptr_t) : ptr(nullptr), ref_count(nullptr) {}

		explicit st_shared_ptr(T* ptr) : ptr(ptr), ref_count(ptr ? new size_t(1) : nullptr) {}

		st_shared_ptr(const st_shared_ptr& other) : ptr(other.ptr), ref_count(other.ref_count)
		{
			if (ref_count)
			{
				++(*ref_count);
			}
		}

		st_shared_ptr(st_shared_ptr&& other) noexcept : ptr(other.ptr), ref_count(other.ref_count)
		{
			other.ptr = nullptr;
			other.ref_count = nullptr;
		}

		st_shared_ptr& operator=(const st_shared_ptr& other)
		{
			if (this != &other)
			{
				reset();
				ptr = other.ptr;
				ref_count = other.ref_count;
				if (ref_count)
				{
					++(*ref_count);
				}
			}
			return *this;
		}

		st_shared_ptr& operator=(st_shared_ptr&& other) noexcept
		{
			if (this != &other)
			{
				reset();
				ptr = other.ptr;
				ref_count = other.ref_count;
				other.ptr = nullptr;
				other.ref_count = nullptr;
			}
			return *this;
		}

		st_shared_ptr& operator=(std::nullptr_t)
		{
			reset();
			return *this;
		}

		~st_shared_ptr()
		{
			reset();
		}

		void reset()
		{
			if (ref_count)
			{
				if (--(*ref_count) == 0)
				{
					delete ptr;
					delete ref_count;
				}
			}
			ptr = nullptr;
			ref_count = nullptr;
		}

		T* get() const { return ptr; }

		T& operator*() const { return *ptr; }

		T* operator->() const { return ptr; }

		explicit operator bool() const { return ptr != nullptr; }

		size_t use_count() const { return ref_count ? *ref_count : 0; }

		bool operator==(std::nullptr_t) const { return ptr == nullptr; }

		bool operator!=(std::nullptr_t) const { return ptr != nullptr; }

	private:
		T* ptr;
		size_t* ref_count;
	};

	template<typename T, typename... Args>
	st_shared_ptr<T> make_shared_st(Args&&... args)
	{
		return st_shared_ptr<T>(new T(std::forward<Args>(args)...));
	}
}