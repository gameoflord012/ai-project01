#include "Config.h"
#include "AgentState.h"

#ifndef DESIRE_TARGET_DISTANCE_MOD
#define DESIRE_TARGET_DISTANCE_MOD 0.1f
#endif // !DESIRE_TARGET_DISTANCE_MOD

#ifndef AGENT_POINT_MOD
#define AGENT_POINT_MOD 1
#endif // !AGENT_POINT_MOD

bool AgentState::operator==(const AgentState& other) const
{
    bool isEqual =
        index == other.index and
        keyMask == other.keyMask and
        point == other.point;

    if (desiredTargets.size() != other.desiredTargets.size())
    {
        return false;
    }

    for (int i = 0; i < desiredTargets.size(); i++)
    {
        if (desiredTargets[i] != other.desiredTargets[i])
        {
            isEqual = false;
            break;
        }
    }

    return isEqual;
}

float AgentState::get_heuristic_value(const Board& board)
{
    float h_value = 0;

    float h_distance = 0;
    float h_point = point;

    if (desiredTargets.size() > 0)
    {
        for (int i = 0; i < desiredTargets.size() - 1; i++)
        {
            int desire_index = desiredTargets[i];
            h_distance += board.estimate_distance(desire_index, index) * DESIRE_TARGET_DISTANCE_MOD;
        }

        h_distance += board.estimate_distance(index, desiredTargets.back());
    }

    h_value = h_distance - h_point * AGENT_POINT_MOD;

    return h_value;
}
