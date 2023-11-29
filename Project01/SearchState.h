#pragma once

#include <functional>
#include <vector>

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

    std::vector<int> desiredTargets[MAX_AGENT_COUNT];

    std::size_t operator()(const SearchState& searchState);
};

bool operator==(const SearchState& left, const SearchState& right);

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
                for (int e : searchState.desiredTargets[i]) hash_combine(hashValue, e);
            }

            return hashValue;
        }
    };
}