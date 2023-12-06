#include "Config.h"

#include "SearchState.h"
#include "Helpers.h"

#ifndef MAIN_AGENT_SCALE
#define MAIN_AGENT_SCALE 1000
#endif // !MAIN_AGENT_SCALE


SearchState::SearchState(const shared_ptr<Board> board)
{
    this->board = board;
}

float SearchState::get_heuristice_value()
{
    float h_value = time;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        float mod = i == 0 ? MAIN_AGENT_SCALE : 0;
        h_value += agents[i].get_heuristic_value(board) * mod;
    }

    return h_value;
}

std::size_t SearchState::operator()(const SearchState& searchState)
{
    size_t hashValue = 0;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        hash_combine(hashValue, searchState.agents[i]);
    }

    return hashValue;
}

bool SearchState::operator()(const SearchState& a, const SearchState& b) const
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
    printf("\nTime elapsed: %ud miliseconds", timeElapsedInMiniSeconds);
}

bool operator==(const SearchState& left, const SearchState& right)
{
    return left.operator==(right);
}
