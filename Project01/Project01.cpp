// Project01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <memory>
#include <queue>

#pragma region DEFINE

#define ROUND_INT(x) ((int)(x + 0.5f))
#define STRING_EQUAL(x, y) (strcmp(x, y) == 0)
#define GET_MASK(X, Y) ((1 << X << 4) | Y)
#define wrap_fscanf_s(...) { if(fscanf_s(__VA_ARGS__) == 0) { printf("invalid input"); assert(false); exit(0); } }

#define MIN_HEAP(T) 
using namespace std;

using byte = char;
template <typename T>
using MinHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;

const int MAX_BOARD_HEIGHT = 3;
const int MAX_AGENT_COUNT = 9;

#pragma endregion

#pragma region STRUCT_AND_CLASS
struct Position
{
    int x, y, z;
};

struct SearchState
{
    Position agentPositions[MAX_AGENT_COUNT];
    int keyMasks[MAX_AGENT_COUNT];
    int cost;

    std::size_t operator()(const SearchState& element) const {
        // You can use a hash function appropriate for your specific type
        return std::hash<int>()(*agentPositions);
    }
};

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
#pragma endregion

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

    Position getPosition(int index)
    {
        return { index / dim.y % dim.x, index % dim.y, index / dim.y / dim.x };
    }

    void setBoardData(Position p, int value)
    {
        gridData[getIndex(p)] = value;
    }

    void setBoardData(Position p, const char * value)
    {
        gridData[getIndex(p)] = getValue(value);
    }

    int getBoardData(Position p)
    {
        return gridData[getIndex(p)];
    }

    int getValue(const char* raw)
    {
        if (STRING_EQUAL(raw, "-1")) return OBSTACLE;
        else if (STRING_EQUAL(raw, "0")) return FLOOR;
        else if (raw[0] == 'A') return GET_MASK(raw[1] - '1', AGENT);
        else if (raw[0] == 'T') return GET_MASK(raw[1] - '1', TARGET);
        else if (raw[0] == 'K') return GET_MASK(raw[1] - '1', KEY);
        else if (raw[0] == 'D') return GET_MASK(raw[1] - '1', DOOR);
    }

    void printBoard()
    {
        for (int k = 0; k < dim.z; k++)
        {
            printf("\nfloor %d", k);
            for (int i = 0; i < dim.x; i++)
            {
                printf("\n");
                for (int j = 0; j < dim.y; j++)
                {
                    printf("%d ", gridData[getIndex({ i, j, k })]);
                }
            }
        }
    }

    bool isCell(Position p, BoardCell c) const
    {
        if (getIndex(p) < 0) return false;
        return (gridData[getIndex(p)] & 0xF) == c;
    }

    vector<int> gridData;
    Position dim;
};

#pragma region VARIABLES

#pragma endregion

typedef pair<int, int> HeapType;
typedef MinHeap<HeapType> SearchHeap;

void search(const Board& board)
{
#pragma region INIT
    vector<int> agentIndexList, targetIndexList;
    agentIndexList.resize(MAX_AGENT_COUNT, -1);
    targetIndexList.resize(MAX_AGENT_COUNT, -1);

    for (int i = 0; i < board.gridData.size(); i++)
    {
        int value = board.gridData[i];
        if ((value & 0xF) == AGENT)
        {
            int p = ROUND_INT(log2(value >> 4));
            agentIndexList[p] = i;
        }
        else if ((value & 0xF) == TARGET)
        {
            int p = ROUND_INT(log2(value >> 4));
            targetIndexList[p] = i;
        }
    }

    printf("\n");
    for (int e : agentIndexList) printf("%d ", e);
    printf("\n");
    for (int e : targetIndexList) printf("%d ", e);
#pragma endregion
    SearchHeap searchHeap;
    vector<SearchState> searchStateList;


    const int DIR_X[] = { 0, 0, 1, -1,  0, -1,  1, -1, 1 };
    const int DIR_Y[] = { 0, 1, 0,  0, -1,  1, -1, -1, 1 };
    const int DIR_SIZE = 5;

    while (searchHeap.size() > 0)
    {
        HeapType top = searchHeap.top();
        searchHeap.pop();

        SearchState state = searchStateList[top.second];

        for (int iagent = 0; iagent < MAX_AGENT_COUNT; iagent++)
        {
            if (agentIndexList[iagent] == -1) continue;

            for (int idir = 0; idir < DIR_SIZE; idir++)
            {
                SearchState nextState = state;

                Position nextPos = {
                    state.agentPositions[iagent].x + DIR_X[idir],
                    state.agentPositions[iagent].y + DIR_Y[idir],
                    state.agentPositions[iagent].z
                };

                if (board.isCell(nextPos, OBSTACLE))
                {
                    continue;
                }

                if (board.isCell(nextPos, FLOOR))
                {
                    // GO FUCKING THERE

                    state.agentPositions[iagent] = nextPos;
                    state.cost += 1;
                }

                searchStateList.push_back(state);
                searchHeap.push({ state.cost, searchStateList.size() - 1 });
            }
        }
    }
}


int main()
{
    shared_ptr<Board> board;

#pragma region READ_BOARD
    int nCols, nRows;
    FILE* inputFile;

    if (fopen_s(&inputFile, "../input/inp.txt", "r") != 0)
    {
        printf("file not found");
        return -1;
    }

    wrap_fscanf_s(inputFile, "%d,", &nCols);
    wrap_fscanf_s(inputFile, "%d", &nRows);

    board = make_shared<Board>(nCols, nRows);

    char buffer[50];
    int curBoardZ = 0;
    while (fscanf_s(inputFile, "\n[%[^]]]", buffer, sizeof(buffer)) != EOF)
    {
        printf("\n%s\n", buffer);
        for (int i = 0; i < nRows; i++)
        {
            for (int j = 0; j < nCols; j++)
            {
                fscanf_s(inputFile, "%[, \n]", buffer, sizeof(buffer));
                wrap_fscanf_s(inputFile, "%[^, \n]", buffer, sizeof(buffer));

                board->setBoardData({ i, j, curBoardZ }, buffer);
                printf("%s|", buffer);
            }
        }
        curBoardZ++;
    }

    board->printBoard();

    fclose(inputFile);
#pragma endregion   

    search(*board);
}