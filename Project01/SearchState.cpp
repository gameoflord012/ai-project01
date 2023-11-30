#include "SearchState.h"

std::size_t SearchState::operator()(const SearchState& searchState)
{
    return std::hash<SearchState>{}(searchState);
}

int SearchState::getHeuristicValue(const Board& board)
{
    float heuristicValue = 0;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        Position p = board.getPosition(agentIndexes[i]);
        for (int e : agentDesiredTargets[i])
        {
            heuristicValue += p.distance(board.getPosition(e));
        }
    }

    return ROUND_INT(heuristicValue);
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
        if (left.agentDesiredTargets[i].size() != right.agentDesiredTargets[i].size()) return false;
        if (left.agentDesiredTargets[i] != right.agentDesiredTargets[i]) return false;
    }
    return true;
}
