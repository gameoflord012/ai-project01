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
   
#include "Config.h"

#include "Position.h"
#include "Board.h"
#include "SearchState.h"
#include "SearchStatePtr.h"
#include "PriorityValue.h"

using namespace std;

template <typename T>
using MinHeap = std::priority_queue < T, std::vector<T>, T > ;
typedef MinHeap<SearchStatePtr> SearchHeap;
typedef unordered_set < SearchStatePtr, hash<SearchStatePtr> > UniqueSet;

bool search(const Board& board, SearchResultData& resultData)
{
    NEW_PRINT_SECTION(SEARCHING)

#pragma region DECLARE_VALUES
    SearchHeap openedList;
    UniqueSet closedList;

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
    SearchStatePtr initialStatePtr = SearchStatePtr(new SearchState());
    SearchState& initialState = initialStatePtr.value();
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        initialState.agents[i].index = boardData.agentIndexList[i];
        initialState.agents[i].keyMask = 0;
    }

    initialState.time = 0;
    initialState.stateIndex = 0;

    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        if (boardData.agentIndexList[i] != -1 && boardData.targetIndexList[i] != -1)
        {
            initialState.agents[i].desiredTargets.push_back(boardData.targetIndexList[i]);
        }
    }

    stateDataList.push_back(initialState);
    openedList.push(initialStatePtr);
#pragma endregion

    while (openedList.size() > 0)
    {
        PriorityValue top = openedList.top();
        openedList.pop();

#pragma region CHECK_STATE_CLOSED_OR_GOAL_STATE
        SearchState state = stateDataList[top.index];
        if (closedList.count(state) > 0) continue;
        else
        {
            closedList.insert(state);
        }


        if (state.agents[0].desiredTargets.size() == 0)
        {
            resultData.finalState = state;
            isPathFound = true;
            break;
        }
       
        /*bool allAgentsArrived = true;

        for (int i = 0; i < MAX_AGENT_COUNT; i++)
        {
            if (state.agents[i].desiredTargets.size() > 0)
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
        }*/
        

#pragma endregion
   
#pragma region PROCESS_NEXT_STATE
        int iagent = state.time % MAX_AGENT_COUNT;

        if (boardData.agentIndexList[iagent] == -1) // agent don't exist
        {
            state.time += 1;
            stateDataList.push_back(state);

            auto priorityValue = top;
            priorityValue.index = stateDataList.size() - 1;

            openedList.push(priorityValue);

            continue;
        }

        auto agentState = state.agents[iagent];

        if (agentState.index == -1) continue;
        if (agentState.desiredTargets.size() == 0) continue;

        Position agentPosition = board.getPosition(agentState.index);

        for (int idir = 0; idir < DIR_SIZE; idir++)
        {
            SearchState nextState = state;
            nextState.time += 1;
            nextState.stateIndex = stateDataList.size();
            nextState.parentStateIndex = state.stateIndex;

            AgentState& nextAgentState = nextState.agents[iagent];

            Position nextAgentPosition = {
                agentPosition.x + DIR_X[idir],
                agentPosition.y + DIR_Y[idir],
                agentPosition.z
            };

            int nextAgentIndex = board.getIndex(nextAgentPosition);

#pragma region CHECK_INVALID_NEXT_POSITION
            if (board.getIndex(nextAgentPosition) == -1) continue;
            if (board.isTile(nextAgentPosition, OBSTACLE)) continue;
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
                nextAgentState.keyMask |= GET_MASK(board.getBoardValue(nextAgentPosition));
            }
#pragma endregion

            bool noObstacle = not board.isTile(nextAgentPosition, OBSTACLE);
            int tileValue = board.getBoardValue(nextAgentPosition);

            bool noDoorOrHasKey =
                not board.isTile(nextAgentPosition, DOOR) or
                VALUE_CONTAIN_MASK(tileValue, nextAgentState.keyMask);

            bool anotherAgentOccured = false;
            for (int jagent = 0; jagent < MAX_AGENT_COUNT; jagent++)
            {
                if (iagent != jagent && nextAgentIndex == state.agents[jagent].index)
                {
                    anotherAgentOccured = true;
                    break;
                }
            }

            bool isTileValid = noObstacle and noDoorOrHasKey and not anotherAgentOccured;

            if (isTileValid)
            {
                nextAgentState.index = board.getIndex(nextAgentPosition); // update new state
                if (nextAgentState.desiredTargets.size() > 0 && nextAgentState.desiredTargets.back() == nextAgentIndex) //Arrive most recent desire target
                {
                    nextAgentState.desiredTargets.pop_back();
                }
            }
            else
            {
                if (board.isTile(nextAgentPosition, DOOR) and not noDoorOrHasKey) //Step on door tile but have to key for the door
                {
                    int requiredKey = GET_MASK_INDEX(tileValue);
                    int keyIndex = boardData.keyIndexList[requiredKey];

                    if (COUNT(nextAgentState.desiredTargets, nextAgentIndex) == 0)
                    {
                        nextAgentState.desiredTargets.push_back(nextAgentIndex);
                    }

                    if (COUNT(nextAgentState.desiredTargets, keyIndex) == 0)
                    {
                        nextAgentState.desiredTargets.push_back(keyIndex);
                    }
                }
            }

            auto priorityValue = 
                PriorityValue{ (float)((nextState.time + MAX_AGENT_COUNT - 1) / MAX_AGENT_COUNT), 0 } + 
                nextState.getHeuristicValue(board);

            stateDataList.push_back(nextState);
            priorityValue.index = stateDataList.size() - 1;

            openedList.push(priorityValue);
        }
#pragma endregion
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_timer);
    resultData.timeElapsedInMiniSeconds = duration.count();
    return isPathFound;
}

void printPathTrace(const Board& board, const vector<SearchState>& stateData, const SearchState& traceState)
{
    if (traceState.parentStateIndex != -1) 
        printPathTrace(board, stateData, stateData[traceState.parentStateIndex]);

    
    for (int i = 0; i < MAX_AGENT_COUNT; i++) if(traceState.agents[i].index != -1)
    {
        Position p = board.getPosition(traceState.agents[i].index);
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

    wrap_fscanf_s(inputFile, "%d,", &nRows);
    wrap_fscanf_s(inputFile, "%d", &nCols);

    board = make_shared<Board>(nRows, nCols);

    char buffer[50];
    int curBoardZ = 0;

    NEW_PRINT_SECTION(INPUT);
    printf("\nRead inputs:");

    while (fscanf_s(inputFile, "\n[%[^]]]", buffer, sizeof(buffer)) != EOF)
    {
        printf("\n%s", buffer);
        for (int i = 0; i < nRows; i++)
        {
            printf("\n");
            for (int j = 0; j < nCols; j++)
            {
                fscanf_s(inputFile, "%[, \n]", buffer, sizeof(buffer));
                wrap_fscanf_s(inputFile, "%[^, \n]", buffer, sizeof(buffer));

                board->setBoardData({ i, j, curBoardZ }, buffer);
                printf("%2s|", buffer);
            }
        }
        curBoardZ++;
    }

    board->printBoard(curBoardZ);

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