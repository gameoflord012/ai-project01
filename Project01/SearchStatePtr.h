#pragma once
#include <memory>

#include "SearchState.h"

struct SearchStatePtr
{
public:
	SearchStatePtr(SearchState * ptr) : _ptr(ptr) {}

	SearchState* ptr() const
	{
		return _ptr.get();
	}

	SearchState& value() const
	{
		return *ptr();
	}

	std::size_t operator()(const SearchStatePtr& ptr)
	{
		return std::hash<SearchState>{}(ptr.value());
	}

private:
	std::shared_ptr<SearchState> _ptr;
};

bool operator==(const SearchStatePtr& left, const SearchStatePtr& right)
{
	return left.value() == right.value();
}
