#include <iostream>
template <typename T,typename Deleter = std::default_delete<T>>
class UniqPointer
{
private:
	T* ptr;
	Deleter deleter;
public:
	explicit UniqPointer(T* ptr) noexcept
	{
		this->ptr = ptr;
		this->deleter = Deleter();
	}
	UniqPointer(const UniqPointer& other) = delete;
	UniqPointer(UniqPointer&& other)
	{
		this->ptr = other.ptr;
		this->deleter = std::move(other.deleter);
		other.ptr = nullptr;
	
	}
	~UniqPointer()
	{
		deleter(ptr);
	}
	UniqPointer& operator=(const UniqPointer& other) = delete;
	UniqPointer& operator=(UniqPointer&& other)
	{
		DeletePtr();
		this->ptr = other.ptr;
		deleter = std::move(other.deleter);
		other.ptr = nullptr;

		return *this;
	}
	T& operator*()
	{
		return *ptr;
	}
	T* operator->()
	{
		return ptr;
	}
	T& operator->()
	{
		return *ptr;
	}
	T* get()
	{
		return ptr;
	}
	T* release()
	{
		T* temp = ptr;
		ptr = nullptr;
		return temp;
	}
	template<typename T,typename ...Args>
	UniqPointer<T> make_unique(Args&& ...args)
	{
		return UniqPointer<T>(new T(std::forward <Args>(args)...));
	}
	void Swap(UniqPointer& other)
	{
		std::swap(ptr, other.ptr);
		std::swap(deleter, other.deleter);
	}
	void DeletePtr(T* p = nullptr)
	{
		delete* ptr;
		ptr = p;
	}
	explicit operator bool() const noexcept
	{
		return ptr != ptr;
	}
};
template<typename T>
class UniqPointer<T[]>
{
private:
	T* ptr;

public:
	// Конструктор
	explicit UniqPointer(T* p = nullptr) noexcept : ptr(p) {}

	// Запрещаем копирование
	UniqPointer(const UniqPointer&) = delete;
	UniqPointer& operator=(const UniqPointer&) = delete;

	// Move-конструктор
	UniqPointer(UniqPointer&& other) noexcept
	{
		this->ptr = other.ptr;
		other.ptr = nullptr;
	}

	// Move-присваивание
	UniqPointer& operator=(UniqPointer&& other) noexcept
	{
		if (this != &other)
		{
			reset();
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}

	// Деструктор
	~UniqPointer()
	{
		delete[] ptr; // 🔥 ключевое отличие
	}

	// Доступ по индексу
	T& operator[](size_t i) const
	{
		return ptr[i];
	}

	// Получить указатель
	T* get() const noexcept { return ptr; }

	// Освободить владение
	T* release() noexcept
	{
		T* temp = ptr;
		ptr = nullptr;
		return temp;
	}

	// Сброс
	void reset(T* p = nullptr) noexcept
	{
		delete[] ptr;
		ptr = p;
	}

	// Проверка
	explicit operator bool() const noexcept
	{
		return ptr != nullptr;
	}
};
template<typename S, typename Deleter = std::default_delete<S>>
class SharedPtr
{
private:
	S* ptr;
	unsigned int* count;
	Deleter deleter;

public:
	SharedPtr(S* p = nullptr, Deleter d = Deleter())
		: ptr(p), deleter(d)
	{
		count = p ? new unsigned int(1) : nullptr;
	}

	SharedPtr(const SharedPtr& other)
	{
		ptr = other.ptr;
		count = other.count;
		deleter = other.deleter;

		if (count)
			++(*count);
	}

	SharedPtr(SharedPtr&& other) noexcept
	{
		ptr = other.ptr;
		count = other.count;
		deleter = std::move(other.deleter);

		other.ptr = nullptr;
		other.count = nullptr;
	}

	SharedPtr& operator=(const SharedPtr& other)
	{
		if (this != &other)
		{
			release();

			ptr = other.ptr;
			count = other.count;
			deleter = other.deleter;

			if (count)
				++(*count);
		}
		return *this;
	}

	SharedPtr& operator=(SharedPtr&& other) noexcept
	{
		if (this != &other)
		{
			release();

			ptr = other.ptr;
			count = other.count;
			deleter = std::move(other.deleter);

			other.ptr = nullptr;
			other.count = nullptr;
		}
		return *this;
	}

	~SharedPtr()
	{
		release();
	}

	void release()
	{
		if (count)
		{
			--(*count);

			if (*count == 0)
			{
				if (ptr)
					deleter(ptr);

				delete count;
			}
		}

		ptr = nullptr;
		count = nullptr;
	}

	S* get() const { return ptr; }

	S& operator*() const { return *ptr; }

	S* operator->() const { return ptr; }

	unsigned int use_count() const
	{
		return count ? *count : 0;
	}
};


