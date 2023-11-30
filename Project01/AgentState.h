#pragma once

#include <vector>


struct AgentState
{
	int index = 0;
	int keyMask = 0;
	int targetCount = 0;

	std::vector<int> desiredTargets;

    bool operator==(const AgentState& other) const
    {
        bool isEqual =
            index == other.index and
            keyMask == other.keyMask and
            targetCount == other.targetCount;

        isEqual = desiredTargets.size() == other.desiredTargets.size();
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
};

template<>
struct hash<AgentState>
{
    std::size_t operator()(const AgentState& agents) const {
        size_t hashValue = 0;

        hash_combine(hashValue, agents.index);
        hash_combine(hashValue, agents.keyMask);
        hash_combine(hashValue, agents.targetCount);
        for (int e : agents.desiredTargets) hash_combine(hashValue, e);

        return hashValue;
    }
};