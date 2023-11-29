#include "SearchState.h"

std::size_t SearchState::operator()(const SearchState& searchState)
{
    return std::hash<SearchState>{}(searchState);
}

bool operator==(const SearchState& left, const SearchState& right)
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (left.agentIndexes[i] != right.agentIndexes[i]) return false;
        if (left.keyMasks[i] != right.keyMasks[i]) return false;
    }

    
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (left.desiredTargets[i].size() != right.desiredTargets[i].size()) return false;
        if (left.desiredTargets[i] != right.desiredTargets[i]) return false;
    }
    return true;
}
