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
    }
    return true;
}
