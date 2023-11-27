#pragma once

#include <functional>

#include "Position.h"

#ifndef MAX_AGENT_COUNT
#define MAX_AGENT_COUNT 9
#endif // !MAX_AGENT_COUNT

struct SearchState
{
    Position agentPositions[MAX_AGENT_COUNT];
    int keyMasks[MAX_AGENT_COUNT];
    int cost;

    std::size_t operator()(const SearchState& searchState);
    bool operator==(const SearchState& other);
};

namespace std
{
    template<>
    struct hash<SearchState>
    {
        std::size_t operator()(const SearchState& searchState) const {
            size_t hashValue = 0;
            for (const Position &e : searchState.agentPositions) hash_combine(hashValue, e);
            for (int e : searchState.keyMasks) hash_combine(hashValue, e);
            return hashValue;
        }
    };
}