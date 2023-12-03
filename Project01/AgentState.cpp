#include "AgentState.h"

bool AgentState::operator==(const AgentState& other) const
{
    bool isEqual =
        index == other.index and
        keyMask == other.keyMask and
        targetCount == other.targetCount;

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
