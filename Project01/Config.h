#pragma once

#ifdef EXPORT
	#define SEARCH_API __declspec(dllexport)
#else
	#define SEARCH_API __declspec(dllimport)
#endif // EXPORT

#define DEFAULT_INPUT_PATH "../input/inp.txt"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Project01"

#define MAX_BOARD_HEIGHT 10
#define MAX_AGENT_COUNT 9
#define MAX_KEY_COUNT 9

#define SUB_AGENT_SCALE 1 // should be 1

#define AGENT_POINT_MOD 10 // greater than 1
#define DESIRE_TARGET_DISTANCE_MOD 0.01f // should lower than 1
#define MAIN_AGENT_SCALE 1 // lower than 1

#define GLOBAL_SEED -1 // -1 for auto generatation
