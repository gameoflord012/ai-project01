#pragma once

#include <memory>

template<typename T>
struct SmartPtr
{
public:
	SmartPtr(const T* ptr)
	{
		_ptr = std::shared_ptr<T>((T*)ptr);
	}

	SmartPtr()
	{
		_ptr = nullptr;
	}

	T* ptr() const
	{
		return _ptr.get();
	}

	T& value() const
	{
		return *ptr();
	}

	std::size_t operator()(const SmartPtr<T>& ptr) const
	{
		return T{}(ptr.value());
	}

	bool operator()(const SmartPtr<T>& a, const SmartPtr<T>& b)
	{
		return T{}(a.value(), b.value());
	}


	bool is_null() const
	{
		return _ptr == nullptr;
	}

private:
	std::shared_ptr<T> _ptr;
};

template<typename T>
bool operator==(const SmartPtr<T>& left, const SmartPtr<T>& right)
{
	return left.value() == right.value();
}




