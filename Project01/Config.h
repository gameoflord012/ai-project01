#pragma once

#ifdef EXPORT
	#define SEARCH_API __declspec(dllexport)
#else
	#define SEARCH_API __declspec(dllimport)
#endif // EXPORT

#define MAX_BOARD_HEIGHT 10
#define MAX_AGENT_COUNT 9
#define MAX_KEY_COUNT 9

#define DESIRE_TARGET_DISTANCE_MOD 0.1f // should lower than 1
#define AGENT_POINT_MOD 10 // greater than 1
#define MAIN_AGENT_SCALE 0.5 // should lower than 1

#define GLOBAL_SEED -1 // -1 for auto generatation
