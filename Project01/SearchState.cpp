#include "Config.h"

#include "SearchState.h"
#include "Helpers.h"

#ifndef MAIN_AGENT_SCALE
#define MAIN_AGENT_SCALE 1000
#endif // !MAIN_AGENT_SCALE

#ifndef MAX_AGENT_COUNT
#define MAX_AGENT_COUNT 9
#endif // !MAX_AGENT_COUNT



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
        h_value += agents[i].get_heuristic_value(*board) * mod;
    }

    return h_value;
}

unsigned int SearchState::operator()(const SearchState& searchState)
{
    unsigned int hashValue = 0;

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

void SearchState::print_state(const Board& board, bool exclude_unchanged_state)
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++) if (agents[i].index != -1)
    {
        if (exclude_unchanged_state && time % MAX_AGENT_COUNT != i)
            continue;

        Position p = board.getPosition(agents[i].index);
        printf("\n\033[1;%dm[A%1d] || Tid=%2d || Floor=%2d || POS=2%d 2%d || time=%2d\033[0m", 
            40 + i, i, 
            agents[i].desiredTargets.size() > 0 ? agents[i].desiredTargets[0] : -1, 
            p.z, p.x, p.y, 
            (time + MAX_AGENT_COUNT - 1) / MAX_AGENT_COUNT);


        if (time % MAX_AGENT_COUNT == i + 1) printf("|<<");
    }
    printf("\n==================================================");
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

int SearchResultData::getPointCount()
{
    int count = 0;
    for (const AgentState& a : finalState->agents)
    {
        count += a.point;
    }
    return count;
}

int SearchResultData::getSearchStateCount()
{
    return statePtrList.size();
}

void SearchResultData::printResult()
{
    printf("\nSearch State Count: %d", getSearchStateCount());
    printf("\nPath cost: %d", getPathCost());
    printf("\nPoint: %d", getPointCount());
    printf("\nTime elapsed: %ud miliseconds", timeElapsedInMiniSeconds);
}

bool operator==(const SearchState& left, const SearchState& right)
{
    return left.operator==(right);
}
