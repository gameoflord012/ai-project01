#include "SearchState.h"
#include "Helpers.h"

std::size_t SearchState::operator()(const SearchState& searchState)
{
    size_t hashValue = 0;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        hash_combine(hashValue, searchState.agents[i]);
    }

    return hashValue;
}

bool SearchState::operator()(const SearchState& a, const SearchState& b)
{
    return a.time > b.time;
}

void SearchState::print_state(const Board& board)
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++) if (agents[i].index != -1)
    {
        Position p = board.getPosition(agents[i].index);
        printf("\n\033[1;%dm[A%1d] Floor=%2d, ROW=%2d, COL=%2d\033[0m, time=%2d", 40 + i, i, p.z, p.x, p.y, time);
    }
}

bool SearchState::operator==(const SearchState& other) const
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (not(agents[i] == other.agents[i]))
        {
            return false;
        }
    }

    if (time % MAX_AGENT_COUNT != other.time % MAX_AGENT_COUNT)
    {
        return false;
    }

    return true;
}

//PriorityValue SearchState::getHeuristicValue(const Board& board)
//{
//    PriorityValue priorityValue;
//    for (int i = 0; i < MAX_AGENT_COUNT; i++)
//    {
//        Position p = board.getPosition(agents[i].index);
//        float expectedCost = 0;
//
//        for (int e : agents[i].desiredTargets)
//        {
//            expectedCost += p.distance(board.getPosition(e));
//        }
//
//        if (i == 0) priorityValue.mainAgentHCost = expectedCost;
//        else
//            priorityValue.subAgentHCost += expectedCost;
//    }
//
//    return priorityValue;
//}

//bool operator==(const SearchState& left, const SearchState& right)
//{
//    for (int i = 0; i < MAX_AGENT_COUNT; i++)
//    {
//        if (not(left.agents[i] == right.agents[i]))
//        {
//            return false;
//        }
//    }
//
//    if (left.time % MAX_AGENT_COUNT != right.time % MAX_AGENT_COUNT)
//    {
//        return false;
//    }
//
//    return true;
//}

int SearchResultData::getPathCost()
{
    return (finalState.value().time + MAX_AGENT_COUNT - 1) / MAX_AGENT_COUNT;
}

int SearchResultData::getSearchStateCount()
{
    return statePtrList.size();
}

void SearchResultData::printResult()
{
    printf("\nSearch State Count: %d", getSearchStateCount());
    printf("\nPath cost: %d", getPathCost());
    printf("\nTime elapsed: %f miliseconds", timeElapsedInMiniSeconds);
}

bool operator==(const SearchState& left, const SearchState& right)
{
    return left.operator==(right);
}
