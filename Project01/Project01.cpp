// Project01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <memory>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <chrono>
   
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

bool search(const Board& board, SearchResultData& resultData)
{
    NEW_PRINT_SECTION(SEARCHING)

#pragma region DECLARE_VALUES
    SearchHeap openedList;
    unordered_set < SearchState, hash<SearchState> > closedList;
    vector<SearchState>& stateDataList = resultData.stateData;
    bool isPathFound = false;
    auto start_timer = std::chrono::high_resolution_clock::now();

    stateDataList.clear();

    const int DIR_X[] = { 0, 0, 1, -1,  0, -1,  1, -1, 1 };
    const int DIR_Y[] = { 0, 1, 0,  0, -1,  1, -1, -1, 1 };
    const int DIR_SIZE = 5;

    BoardData boardData = board.getBoardData();
    boardData.printBoardData();
#pragma endregion

#pragma region INIT_OPENED_LIST
    SearchState initialState;
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        initialState.agentIndexes[i] = boardData.agentIndexList[i];
        initialState.keyMasks[i] = 0;
    }

    initialState.cost = 0;
    initialState.stateIndex = 0;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (boardData.agentIndexList[i] != -1 && boardData.targetIndexList[i] != -1)
        {
            initialState.agentDesiredTargets[i].push_back(boardData.targetIndexList[i]);
        }
    }

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

        bool allAgentsArrived = true;
        for (int i = 0; i < MAX_AGENT_COUNT; i++)
        {
            if (state.agentDesiredTargets[i].size() > 0)
            {
                allAgentsArrived = false;
                break;
            }
        }

        if (allAgentsArrived)
        {
            resultData.finalState = state;
            isPathFound = true;
            break;
        }

#pragma endregion

        for (int iagent = 0; iagent < MAX_AGENT_COUNT; iagent++)
        {
            if (boardData.agentIndexList[iagent] == -1) continue;
            if (state.agentIndexes[iagent] == -1) continue;
            if (state.agentDesiredTargets[iagent].size() == 0) continue;

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

                int nextAgentIndex = board.getIndex(nextAgentPosition);

#pragma region CHECK_INVALID_NEXT_POSITION
                if (board.getIndex(nextAgentPosition) == -1) continue;

                if (board.isTile(nextAgentPosition, OBSTACLE))
                {
                    continue;
                }
#pragma endregion

#pragma region UPDATE_STAIR_POSITION
                if (DIR_X[idir] == 0 and DIR_Y[idir] == 0)
                {
                    if (board.isTile(nextAgentPosition, STAIR_UP))
                    {
                        nextAgentPosition.z += 1;
                    }
                    else if (board.isTile(nextAgentPosition, STAIR_DOWN))
                    {
                        nextAgentPosition.z -= 1;
                    }
                }

                if (nextAgentIndex == -1) //out of board
                {
                    continue;
                }
#pragma endregion

#pragma region UPDATE_KEY_MASK
                if (board.isTile(nextAgentPosition, KEY))
                {
                    nextState.keyMasks[iagent] |= GET_MASK(board.getBoardValue(nextAgentPosition));
                }
#pragma endregion

                bool noObstacle = not board.isTile(nextAgentPosition, OBSTACLE);
                int tileValue = board.getBoardValue(nextAgentPosition);

                bool noDoorOrHasKey =
                    not board.isTile(nextAgentPosition, DOOR) or
                    VALUE_CONTAIN_MASK(tileValue, nextState.keyMasks[iagent]);

                bool isTileValid = noObstacle and noDoorOrHasKey;

                if (isTileValid)
                {
                    nextState.agentIndexes[iagent] = board.getIndex(nextAgentPosition); // update new state
                    if (nextState.agentDesiredTargets[iagent].size() > 0 && nextState.agentDesiredTargets[iagent].back() == nextAgentIndex) //Arrive most recent desire target
                    {
                        nextState.agentDesiredTargets[iagent].pop_back();
                    }
                }
                else
                {
                    if (board.isTile(nextAgentPosition, DOOR) and not noDoorOrHasKey) //Step on door tile but have to key for the door
                    {
                        int requiredKey = GET_MASK_INDEX(tileValue);
                        int keyIndex = boardData.keyIndexList[requiredKey];

                        if (COUNT(nextState.agentDesiredTargets[iagent], nextAgentIndex) == 0)
                        {
                            nextState.agentDesiredTargets[iagent].push_back(nextAgentIndex);
                        }

                        if (COUNT(nextState.agentDesiredTargets[iagent], keyIndex) == 0)
                        {
                            nextState.agentDesiredTargets[iagent].push_back(keyIndex);
                        }
                    }
                }

                stateDataList.push_back(nextState);
                openedList.push({ nextState.cost + nextState.getHeuristicValue(board), stateDataList.size() - 1 });
            }
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_timer);
    resultData.timeElapsedInMiniSeconds = duration.count();
    return isPathFound;
}

void printPathTrace(const Board& board, const vector<SearchState>& stateData, const SearchState& traceState)
{
    if (traceState.parentStateIndex != -1) 
        printPathTrace(board, stateData, stateData[traceState.parentStateIndex]);

    
    for (int i = 0; i < MAX_AGENT_COUNT; i++) if(traceState.agentIndexes[i] != -1)
    {
        Position p = board.getPosition(traceState.agentIndexes[i]);
        printf("\n[A%1d] Floor %2d, ROW = %3d, COL = %3d", i, p.z, p.x, p.y);
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

    NEW_PRINT_SECTION(INPUT);
    printf("\nRead inputs:");

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

    SearchResultData resultData;

    bool isSearchSuccess = search(*board, resultData);

    NEW_PRINT_SECTION(RESULT)
    if (isSearchSuccess)
    {
        printf("\npath go:");
        printPathTrace(*board, resultData.stateData, resultData.finalState);

        resultData.printResult();
    }
    else
    {
        printf("\nno solution found");
    }
    
}