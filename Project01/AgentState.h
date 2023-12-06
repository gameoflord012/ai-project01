#pragma once

#include <vector>

#include "Board.h"
#include "HashUtils.h"


struct AgentState
{
	int index = 0;
	int keyMask = 0;
	int point = 0;

	std::vector<int> desiredTargets;

    bool operator==(const AgentState& other) const;
    float get_heuristic_value(const Board& board);

    std::size_t operator()(const AgentState& agents) const {
        size_t hashValue = 0;

        hash_combine(hashValue, agents.index);
        hash_combine(hashValue, agents.keyMask);
        hash_combine(hashValue, agents.point);

        for (int e : agents.desiredTargets) hash_combine(hashValue, e);

        return hashValue;
    }
};