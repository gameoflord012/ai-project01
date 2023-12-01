#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "Config.h"

#include "Board.h"
#include "Position.h"
#include "PriorityValue.h"
#include "AgentState.h"

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

    int stateIndex = -1;
    int parentStateIndex = -1;

    weak_ptr<SearchState> parent;

    std::size_t operator()(const SearchState& searchState);

    PriorityValue getHeuristicValue(const Board& board);
};

bool operator==(const SearchState& left, const SearchState& right);

struct SearchResultData
{
    std::vector<SearchStatePtr> stateData;
    SearchState finalState;
    
    size_t timeElapsedInMiniSeconds = 0;

    int getPathCost()
    {
        return (finalState.time + MAX_AGENT_COUNT - 1) / MAX_AGENT_COUNT;
    }

    int getSearchStateCount()
    {
        return stateData.size();
    }

    void printResult()
    {
        printf("\nSearch State Count: %d", getSearchStateCount());
        printf("\nPath cost: %d", getPathCost());
        printf("\nTime elapsed: %f miliseconds", timeElapsedInMiniSeconds);
    }
};

namespace std
{
    template<>
    struct hash<SearchState>
    {
        std::size_t operator()(const SearchState& searchState) const {
            size_t hashValue = 0;
            
            for (int i = 0; i < MAX_AGENT_COUNT; i++)
            {
                hash_combine(hashValue, searchState.agents[i]);
            }

            return hashValue;
        }
    };
}