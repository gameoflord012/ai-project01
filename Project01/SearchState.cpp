#include "Config.h"

#include <vector>

#include "SearchState.h"
#include "Search.h"
#include "Helpers.h"

#ifndef MAIN_AGENT_SCALE
#define MAIN_AGENT_SCALE 1000
#endif // !MAIN_AGENT_SCALE

#ifndef SUB_AGENT_SCALE
#define SUB_AGENT_SCALE 1
#endif // !SUB_AGENT_SCALE


#ifndef MAX_AGENT_COUNT
#define MAX_AGENT_COUNT 9
#endif // !MAX_AGENT_COUNT



SearchState::SearchState(const shared_ptr<Board> board)
{
    this->board = board;
    parent.reset();
}

float SearchState::get_heuristice_value()
{
    if (not global::use_heuristic)
    {
        return 1;
    }

    float h_value = time;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        float mod = i == 0 ? MAIN_AGENT_SCALE : SUB_AGENT_SCALE;
        h_value += agents[i].get_heuristic_value(*board) * mod;
    }

    return h_value; // time + tong heuristic cua agents
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

void SearchState::print_state(bool exclude_unchanged_state) const
{
    for (int i = 0; i < MAX_AGENT_COUNT; i++) if (agents[i].index != -1)
    {
        Position p = board->getPosition(agents[i].index);
        printf("\n\033[1;%dm[A%1d] || Tid=%2d || Floor=%2d || POS=%2d %2d || time=%2d\033[0m", 
            40 + i, i, 
            agents[i].desiredTargets.size() > 0 ? agents[i].desiredTargets[0] : -1, 
            p.z, p.x, p.y, 
            (time + MAX_AGENT_COUNT - 1) / MAX_AGENT_COUNT);


        if ((time - 1 + MAX_AGENT_COUNT) % MAX_AGENT_COUNT == i) printf("|<<");
    }
    printf("\n==================================================");
}

void SearchState::trace_state(std::vector<SearchState>& result, shared_ptr<SearchState> statePtr)
{
    if (not statePtr->parent.expired())
        trace_state(result, statePtr->parent.lock());
    else
        result.clear();

    result.push_back(*statePtr);
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

vector<SearchState> SearchResultData::get_path(bool exclude_unchanged_state)
{
    vector<SearchState> trace;
    finalState->trace_state(trace, finalState.shared_ptr());

    if (not exclude_unchanged_state) return trace;

    vector<SearchState> result;

    for (const SearchState& state : trace)  if (state.time != 0)
    {
            bool should_exclude = true;

            for (int i = 0; i < MAX_AGENT_COUNT; i++) if (
                state.agents[i].index != -1 &&
                (state.time - 1 + MAX_AGENT_COUNT) % MAX_AGENT_COUNT == i)
            {
                should_exclude = false;
                break;
            }

            if (not should_exclude) result.push_back(state);
    }

    return result;
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

void SearchResultData::print_path()
{
    for (const SearchState& state : get_path())
    {
        state.print_state(true);
    }
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
