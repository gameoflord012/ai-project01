// Project01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <memory>
#include <queue>
#include <unordered_set>

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

bool search(const Board& board, vector<SearchState> &stateDataList, SearchState& returnSearchState)
{
#pragma region READ_BOARD
    printf("\nStart Searching ...");

    int agentIndexList[MAX_AGENT_COUNT], targetIndexList[MAX_AGENT_COUNT];
    fill(agentIndexList, agentIndexList + MAX_AGENT_COUNT, -1);
    fill(targetIndexList, targetIndexList + MAX_AGENT_COUNT, -1);

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
    
#pragma region DECLARE_VALUES
    SearchHeap openedList;
    unordered_set < SearchState, hash<SearchState> > closedList;
    stateDataList.clear();

    const int DIR_X[] = { 0, 0, 1, -1,  0, -1,  1, -1, 1 };
    const int DIR_Y[] = { 0, 1, 0,  0, -1,  1, -1, -1, 1 };
    const int DIR_SIZE = 5;
#pragma endregion

#pragma region INIT_OPENED_LIST
    SearchState initialState;
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        initialState.agentIndexes[i] = agentIndexList[i];
        initialState.keyMasks[i] = 0;
    }

    initialState.cost = 0;
    initialState.stateIndex = 0;

    stateDataList.push_back(initialState);
    openedList.push({ 0, 0 });
#pragma endregion

    while (openedList.size() > 0)
    {
        HeapType top = openedList.top();
        openedList.pop();

#pragma region CHECK_STATE_CLOSED_OR_GOAL_STATE
        SearchState state = stateDataList[top.second];
        if (closedList.count(state) > 0) continue;
        else
        {
            closedList.insert(state);
        }

        if (state.agentIndexes[0] == targetIndexList[0]) //A1 arrived T1
        {
            returnSearchState = state;
            return true;
        }
#pragma endregion

        for (int iagent = 0; iagent < MAX_AGENT_COUNT; iagent++)
        {
            if (agentIndexList[iagent] == -1) continue;
            if (state.agentIndexes[iagent] == -1) continue;

            Position agentPosition = board.getPosition(state.agentIndexes[iagent]);

            for (int idir = 0; idir < DIR_SIZE; idir++)
            {
                SearchState nextState = state;
                nextState.cost += 1;
                nextState.stateIndex = stateDataList.size();
                nextState.parentStateIndex = state.stateIndex;

                Position nextAgentPosition = {
                    agentPosition.x + DIR_X[idir],
                    agentPosition.y + DIR_Y[idir],
                    agentPosition.z
                };

#pragma region CHECK_INVALID_NEXT_POSITION
                if (board.getIndex(nextAgentPosition) == -1) continue;

                if (board.isCell(nextAgentPosition, OBSTACLE))
                {
                    continue;
                }
#pragma endregion

#pragma region UPDATE_STAIR_POSITION
                if (DIR_X[idir] == 0 and DIR_Y[idir] == 0)
                {
                    if (board.isCell(nextAgentPosition, STAIR_UP))
                    {
                        nextAgentPosition.z += 1;
                    }
                    else if (board.isCell(nextAgentPosition, STAIR_DOWN))
                    {
                        nextAgentPosition.z -= 1;
                    }
                }

                if (board.getIndex(nextAgentPosition) == -1) //out of board
                {
                    continue;
                }
#pragma endregion

#pragma region UPDATE_KEY_MASK
                if (board.isCell(nextAgentPosition, KEY))
                {
                    nextState.keyMasks[iagent] |= GET_MASK(board.getBoardValue(nextAgentPosition));
                }
#pragma endregion

                bool noObstacle = not board.isCell(nextAgentPosition, OBSTACLE);
                bool noDoorOrHasKey =
                    not board.isCell(nextAgentPosition, DOOR) or
                    VALUE_CONTAIN_MASK(board.getBoardValue(nextAgentPosition), nextState.keyMasks[iagent]);

                bool isTileValid = noObstacle and noDoorOrHasKey;

                if (isTileValid)
                {
                    nextState.agentIndexes[iagent] = board.getIndex(nextAgentPosition); // update new state
                }
                else
                {
                    continue;
                }

                stateDataList.push_back(nextState);
                openedList.push({ nextState.cost, stateDataList.size() - 1 });
            }
        }
    }

    return false;
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

    vector<SearchState> searchDataList;
    SearchState searchResult;

    if (search(*board, searchDataList, searchResult))
    {
        printf("\ncost of search: %d", searchResult.cost);
    }
    else
    {
        printf("\nno solution found");
    }
    
}