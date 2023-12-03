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
    bool operator()(const SearchState& a, const SearchState& b);


    bool operator==(const SearchState& other) const;
    
    /*PriorityValue getHeuristicValue(const Board& board);*/
};

typedef SmartPtr<SearchState> StatePtr;
//template class SmartPtr<SearchState>;
//		if (a.mainAgentHCost != b.mainAgentHCost) return a.mainAgentHCost > b.mainAgentHCost;
//		return a.subAgentHCost > b.subAgentHCost;
//	}
//
//bool operator==(const StatePtr& left, const StatePtr& right)
//{
//    return left.operator==(right);
//}

struct SearchResultData
{
    std::vector<StatePtr> statePtrList;
    StatePtr finalState;
    size_t timeElapsedInMiniSeconds = 0;

    int getPathCost();
    int getSearchStateCount();
    void printResult();
};