#pragma once
#include <vector>

#include <set>
#include <random>

#include "Position.h"

#define GET_BOARD_VALUE(X, Y) ((1 << X << 4) | Y)
#define GET_MASK(X) (X >> 4)
#define GET_MASK_INDEX(M) (FIRST_ON_BIT_POSITION(M >> 4))
#define VALUE_CONTAIN_MASK(V, M) (((V >> 4) & M) != 0)

#ifndef MAX_BOARD_HEIGHT
#define MAX_BOARD_HEIGHT 10
#endif // !MAX_BOARD_HEIGHT

#ifndef MAX_AGENT_COUNT
#define MAX_AGENT_COUNT 9
#endif // !MAX_AGENT_COUNT

#ifndef MAX_KEY_COUNT
#define MAX_KEY_COUNT 9
#endif // !MAX_KEY_COUNT

#ifndef RANDOM_SEED
#define RANDOM_SEED -1
#endif // !RANDOM_SEED

using namespace std;

enum BoardTile
{
    FLOOR,
    AGENT,
    TARGET,
    DOOR,
    KEY,
    OBSTACLE,
    STAIR_UP,
    STAIR_DOWN,
};

struct BoardData
{
    int agentIndexList[MAX_AGENT_COUNT];
    int targetIndexList[MAX_AGENT_COUNT];

    vector<int> floor_index_list[MAX_BOARD_HEIGHT];

    int keyIndexList[MAX_KEY_COUNT];
    int doorIndexList[MAX_KEY_COUNT];

    int generate_random_target_index(unsigned int seed, int level, const set<int>& exclude_list = set<int>());

    void printBoardData();
};

struct Board
{
    Board(int nRows, int nCols, int nHeight = MAX_BOARD_HEIGHT);

    void setBoardData(Position p, int value);
    void setBoardData(Position p, const char* value);
    
    int getBoardValue(Position p) const;
    int getValue(const char* raw);
    int getIndex(Position p) const;
    BoardData getBoardData() const;
    Position getPosition(int index) const;

    bool isTile(Position p, BoardTile c) const;
    float estimate_distance(int ia, int ib, bool is_euclidean = false) const;

    void printBoard(int maxHeight = -1);

    std::vector<int> gridData;
    Position dim;
};