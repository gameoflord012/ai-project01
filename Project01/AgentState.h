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
	unsigned int operator()(const AgentState& agents) const;

    float get_heuristic_value(const Board& board);
};