// Project01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <memory>
#include <queue>

#define MAX_BOARD_HEIGHT 3
#define MAX_AGENT_COUNT 9

#include "Position.h"
#include "Board.h"
#include "SearchState.h"

using namespace std;

template <typename T>
using MinHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;
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