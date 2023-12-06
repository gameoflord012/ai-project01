#pragma once

#include <memory>

template<typename T>
struct SmartPtr
{
public:
	SmartPtr(const T* raw_ptr)
	{
		_shared_ptr = std::shared_ptr<T>((T*)raw_ptr);
	}

	SmartPtr(const SmartPtr& ptr)
	{
		_shared_ptr = ptr._shared_ptr;
	}

	SmartPtr()
	{
		_shared_ptr = nullptr;
	}

	std::shared_ptr<T> shared_ptr() const
	{
		return _shared_ptr;
	}

	T& value() const
	{
		return *_shared_ptr;
	}

	T* operator->() const
	{
		return _shared_ptr.get();
	}

	std::size_t operator()(const SmartPtr<T>& ptr) const
	{
		return ptr.value()(ptr.value());
	}

	bool operator()(const SmartPtr<T>& a, const SmartPtr<T>& b) const
	{
		return a.value()(a.value(), b.value());
	}

	bool operator==(const SmartPtr<T>& other) const
	{
		return this->value() == other.value();
	}

	bool is_null() const
	{
		return _shared_ptr == nullptr;
	}

private:
	std::shared_ptr<T> _shared_ptr;
};



