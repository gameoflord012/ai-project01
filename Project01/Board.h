#pragma once
#include <vector>

#include "Position.h"
#include "Helpers.h"

#define GET_BOARD_VALUE(X, Y) ((1 << X << 4) | Y)
#define GET_MASK(X) (X >> 4)
#define VALUE_CONTAIN_MASK(V, M) (((V >> 4) & M) != 0)

#ifndef MAX_BOARD_HEIGHT
#define MAX_BOARD_HEIGHT 10
#endif // !MAX_BOARD_HEIGHT

enum BoardCell
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

struct Board
{
    Board(int nRows, int nCols, int nHeight = MAX_BOARD_HEIGHT)
    {
        dim.x = nRows;
        dim.y = nCols;
        dim.z = nHeight;

        gridData.resize(dim.x * dim.y * dim.z);
    }

    int getIndex(Position p) const
    {
        if (p.x < 0 || p.x >= dim.x ||
            p.y < 0 || p.y >= dim.y ||
            p.z < 0 || p.z >= dim.z) return -1;

        return (p.z * dim.x + p.x) * dim.y + p.y;
    }

    Position getPosition(int index) const
    {
        return { index / dim.y % dim.x, index % dim.y, index / dim.y / dim.x };
    }

    void setBoardData(Position p, int value)
    {
        gridData[getIndex(p)] = value;
    }

    void setBoardData(Position p, const char* value)
    {
        gridData[getIndex(p)] = getValue(value);
    }

    int getBoardValue(Position p) const
    {
        return gridData[getIndex(p)];
    }

    int getValue(const char* raw)
    {
        if (STRING_EQUAL(raw, "-1"))        return OBSTACLE;
        else if (STRING_EQUAL(raw, "0"))    return FLOOR;
        else if (STRING_EQUAL(raw, "UP"))   return STAIR_UP;
        else if (STRING_EQUAL(raw, "DO"))   return STAIR_DOWN;
        else if (raw[0] == 'A') return GET_BOARD_VALUE(raw[1] - '1', AGENT);
        else if (raw[0] == 'T') return GET_BOARD_VALUE(raw[1] - '1', TARGET);
        else if (raw[0] == 'K') return GET_BOARD_VALUE(raw[1] - '1', KEY);
        else if (raw[0] == 'D') return GET_BOARD_VALUE(raw[1] - '1', DOOR);
    }

    void printBoard()
    {
        NEW_PRINT_SECTION(BOARD INITIALIZATION)
        printf("\nGenerated board value: ");
        for (int k = 0; k < dim.z; k++)
        {
            printf("\nfloor %2d", k);
            for (int i = 0; i < dim.x; i++)
            {
                printf("\n");
                for (int j = 0; j < dim.y; j++)
                {
                    printf("%3d|", gridData[getIndex({ i, j, k })]);
                }
            }
        }
    }

    bool isCell(Position p, BoardCell c) const
    {
        if (getIndex(p) < 0) return false;
        return (gridData[getIndex(p)] & 0xF) == c;
    }

    std::vector<int> gridData;
    Position dim;
};