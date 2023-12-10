#pragma once

#include <vector>

#include "Board.h"
#include "HashUtils.h"

struct AgentState
{
	int index = -1; // 1d position in board
	int keyMask = 0; // 1001 key 0 and key 3
	int point = 0;

	std::vector<int> desiredTargets; // T2 D2 K2 - stack 

	bool operator==(const AgentState& other) const;
	unsigned int operator()(const AgentState& agents) const;

	float get_heuristic_value(const Board& board) const;
};