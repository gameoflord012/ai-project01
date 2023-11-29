#pragma once

#include <functional>
#include <vector>

#include "Board.h"
#include "Position.h"

#ifndef MAX_AGENT_COUNT
#define MAX_AGENT_COUNT 9
#endif // !MAX_AGENT_COUNT

#ifndef MAX_AGENT_COUNT
#define MAX_KEY_COUNT 9
#endif // !MAX_AGENT_COUNT

struct SearchState
{
    int agentIndexes[MAX_AGENT_COUNT];
    int keyMasks[MAX_AGENT_COUNT];
    int cost = 0;

    int stateIndex = -1;
    int parentStateIndex = -1;

    std::vector<int> agentDesiredTargets[MAX_AGENT_COUNT];

    std::size_t operator()(const SearchState& searchState);

    int getHeuristicValue(const Board& board);
};

bool operator==(const SearchState& left, const SearchState& right);

struct SearchResultData
{
    std::vector<SearchState> stateData;
    SearchState finalState;
    
    size_t timeElapsedInMiniSeconds = 0;

    int getPathCost()
    {
        return finalState.cost;
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
            for (int e : searchState.agentIndexes) hash_combine(hashValue, e);
            for (int e : searchState.keyMasks) hash_combine(hashValue, e);

            for (int i = 0; i < MAX_AGENT_COUNT; i++)
            {
                for (int e : searchState.agentDesiredTargets[i]) hash_combine(hashValue, e);
            }

            return hashValue;
        }
    };
}