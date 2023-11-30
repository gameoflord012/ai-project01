#include "SearchState.h"
#include "Helpers.h"

std::size_t SearchState::operator()(const SearchState& searchState)
{
    return std::hash<SearchState>{}(searchState);
}

PriorityValue SearchState::getHeuristicValue(const Board& board)
{
    PriorityValue priorityValue;
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        Position p = board.getPosition(agents[i].index);
        float expectedCost = 0;

        for (int e : agents[i].desiredTargets)
        {
            expectedCost += p.distance(board.getPosition(e));
        }

        if (i == 0) priorityValue.mainAgentHCost = expectedCost * MAX_AGENT_COUNT;
        else
            priorityValue.subAgentHCost += expectedCost;
    }

    return priorityValue;
}

bool operator==(const SearchState& left, const SearchState& right)
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (not(left.agents[i] == right.agents[i]))
        {
            return false;
        }
    }

    if (left.time % MAX_AGENT_COUNT != right.time % MAX_AGENT_COUNT)
    {
        return false;
    }

    return true;
}
