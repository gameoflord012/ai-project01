#pragma once

struct PriorityValue
{
	float mainAgentHCost = 0;
	float subAgentHCost = 0;
	int index = 0;

	PriorityValue operator+(const PriorityValue& other)
	{
		return { mainAgentHCost + other.mainAgentHCost, subAgentHCost + other.subAgentHCost };
	}

	bool operator()(const PriorityValue& a, const PriorityValue& b) const {
		if (a.mainAgentHCost != b.mainAgentHCost) return a.mainAgentHCost > b.mainAgentHCost;
		return a.subAgentHCost > b.subAgentHCost;
	}
};

