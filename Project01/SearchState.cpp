#include "SearchState.h"

std::size_t SearchState::operator()(const SearchState& searchState)
{
    return std::hash<SearchState>{}(searchState);
}

bool SearchState::operator==(const SearchState& other)
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (!agentPositions[i].equals(other.agentPositions[i])) return false;
    }
    return true;
}
