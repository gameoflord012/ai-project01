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

struct SearchState
{
    SearchState(const shared_ptr<Board> board);

    // use for compare
    AgentState agents[MAX_AGENT_COUNT];
    int time = 0;

    // not use for compare
    SmartPtr<SearchState> parent;
    shared_ptr<Board> board;

    float get_heuristice_value();

    unsigned int operator()(const SearchState& searchState);
    bool operator==(const SearchState& other) const;
    bool operator()(const SearchState& a, const SearchState& b) const; // State Comparator

    void print_state(const Board& board, bool exclude_unchanged_state = false);
};

typedef SmartPtr<SearchState> StatePtr;

struct SearchResultData
{
    std::vector<StatePtr> statePtrList;
    StatePtr finalState;
    unsigned int timeElapsedInMiniSeconds = 0;

    int getPathCost();
    int getPointCount();
    int getSearchStateCount();
    void printResult();
};