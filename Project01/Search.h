#pragma once

#include <algorithm>
#include <unordered_set>
#include <queue>

#include "Config.h"

#include "Position.h"
#include "Board.h"
#include "SearchState.h"
#include "SmartPtr.h"

using namespace std;

namespace algorithm
{
	template <typename T>
	using MinHeap = priority_queue < T, std::vector<T>, T >;
	typedef MinHeap<StatePtr> SearchHeap;
	typedef unordered_set < StatePtr, StatePtr > UniqueSet;

	bool SEARCH_API search(const shared_ptr<Board> board, SearchResultData& resultData);
	bool SEARCH_API read(shared_ptr<Board>& board, const char* filepath);

	extern bool use_heuristic;
}