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
    AgentState agents[MAX_AGENT_COUNT];

    int time = 0;

    /*int stateIndex = -1;
    int parentStateIndex = -1;*/

    SmartPtr<SearchState> parent;

    std::size_t operator()(const SearchState& searchState);
    bool operator==(const SearchState& other) const;
    bool operator()(const SearchState& a, const SearchState& b); // State Comparator
};

typedef SmartPtr<SearchState> StatePtr;

struct SearchResultData
{
    std::vector<StatePtr> statePtrList;
    StatePtr finalState;
    size_t timeElapsedInMiniSeconds = 0;

    int getPathCost();
    int getSearchStateCount();
    void printResult();
};