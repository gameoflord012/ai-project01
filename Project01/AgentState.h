#pragma once

#include <vector>

#include "HashUtils.h"

struct AgentState
{
	int index = 0;
	int keyMask = 0;
	int targetCount = 0;

	std::vector<int> desiredTargets;

    bool operator==(const AgentState& other) const;
};

template<>
struct std::hash<AgentState>
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