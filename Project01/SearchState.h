#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "Config.h"

#include "Board.h"
#include "AgentState.h"
#include "SmartPtr.h"

#ifndef MAX_AGENT_COUNT
#define MAX_AGENT_COUNT 9
#endif // !MAX_AGENT_COUNT

#ifndef MAX_KEY_COUNT
#define MAX_KEY_COUNT 9
#endif // !MAX_AGENT_COUNT

SEARCH_API struct SearchState
{
    SearchState(const shared_ptr<Board> board);

    // use for compare
    AgentState agents[MAX_AGENT_COUNT];
    int time = 0;

    // not use for compare
    weak_ptr<SearchState> parent;
    shared_ptr<Board> board;

    float get_heuristice_value() const;

    unsigned int operator()(const SearchState& searchState) const;
    bool operator==(const SearchState& other) const;
    bool operator()(const SearchState& a, const SearchState& b) const; // State Comparator

    void print_state(bool exclude_unchanged_state = false) const;
    void trace_state(std::vector<SearchState>& result, const shared_ptr<SearchState>& statePtr);
};

typedef SmartPtr<SearchState> StatePtr;

SEARCH_API struct SearchResultData
{
    std::vector<StatePtr> statePtrList;
    StatePtr finalState;
    unsigned int timeElapsedInMiniSeconds = 0;

    vector<SearchState> get_path(bool exclude_unchanged_state = true);

    int getPathCost();
    int getPointCount();
    int getSearchStateCount();
    void print_path();
    void printResult();
};