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
#include <functional>   
#include <chrono>

#include "Config.h"

#include "Position.h"
#include "Board.h"
#include "SearchState.h"
#include "SmartPtr.h"
#include "Helpers.h"


using namespace std;

template <typename T>
using MinHeap = std::priority_queue < T, std::vector<T>, T > ;
typedef MinHeap<StatePtr> SearchHeap;
typedef unordered_set < StatePtr, StatePtr > UniqueSet;

bool search(const shared_ptr<Board> board, SearchResultData& resultData)
{
    NEW_PRINT_SECTION(SEARCHING)

    auto start_timer = std::chrono::high_resolution_clock::now();
    unsigned int search_seed = generate_seed(std::string("SEARCH_SEED_MESSI_IS_DA_G.O.A.T"));
    //unsigned int search_seed = 0;

#pragma region DECLARE_VALUES
    SearchHeap openedList;
    UniqueSet closedList;

    vector<StatePtr>& statePtrList = resultData.statePtrList;
    statePtrList.clear();

    bool isPathFound = false;

    const int DIR_X[] = { 0, 0, 1, -1,  0, -1,  1, -1, 1 };
    const int DIR_Y[] = { 0, 1, 0,  0, -1,  1, -1, -1, 1 };
    const int DIR_SIZE = 5;

    BoardData boardData = board->getBoardData();
    boardData.printBoardData();
#pragma endregion

#pragma region INIT_INITIAL_STATE_AND_OPENED_LIST
    StatePtr initialStatePtr(new SearchState(board));
    
    for (int i = 0; i < MAX_AGENT_COUNT; i++)
    {
        initialStatePtr.value().agents[i].index = boardData.agentIndexList[i];
        initialStatePtr.value().agents[i].keyMask = 0;
    }

    initialStatePtr.value().time = 0;

    for (int i = 0; i < MAX_AGENT_COUNT; i++) // add initial targets to desire target list
    {
        if (boardData.agentIndexList[i] != -1 && boardData.targetIndexList[i] != -1)
        {
            initialStatePtr.value().agents[i].desiredTargets.push_back(boardData.targetIndexList[i]);
        }
    }

    openedList.push(initialStatePtr);
#pragma endregion

    while (openedList.size() > 0)
    {
        StatePtr statePtr = openedList.top();
        //statePtr->print_state(board);
        openedList.pop();

#pragma region CHECK_STATE_CLOSED_OR_GOAL_STATE
        if (closedList.count(statePtr) > 0) continue;
        else
        {
            closedList.insert(statePtr);
        }

        statePtrList.push_back(statePtr);

        if (statePtr->agents[0].desiredTargets.size() == 0) // state is final state
        {
            resultData.finalState = statePtr;
            isPathFound = true;
            break;
        }
       
       /* bool allAgentsArrived = true;

        for (int i = 0; i < MAX_AGENT_COUNT; i++)
        {
            if (statePtr->agents[i].desiredTargets.size() > 0)
            {
                allAgentsArrived = false;
                break;
            }
        }

        if (allAgentsArrived)
        {
            resultData.finalState = statePtr;
            isPathFound = true;
            break;
        }*/
        

#pragma endregion
   
#pragma region PROCESS_NEXT_STATE
        int iagent = statePtr->time % MAX_AGENT_COUNT;

        if (boardData.agentIndexList[iagent] == -1) // agent don't exist
        {
            StatePtr nextStatePtr(new SearchState(statePtr.value()));

            nextStatePtr->time += 1;
            nextStatePtr->parent = statePtr;
            openedList.push(nextStatePtr);

            continue;
        }

        const AgentState& agentState = statePtr->agents[iagent];

        if (agentState.index == -1) continue;
        if (agentState.desiredTargets.size() == 0) continue;

        Position agentPosition = board->getPosition(agentState.index);

        for (int idir = 0; idir < DIR_SIZE; idir++)
        {
            StatePtr nextStatePtr(new SearchState(statePtr.value()));

            nextStatePtr->time += 1;
            nextStatePtr->parent = statePtr;

            AgentState& nextAgentState = nextStatePtr->agents[iagent];

            Position nextAgentPosition = {
                agentPosition.x + DIR_X[idir],
                agentPosition.y + DIR_Y[idir],
                agentPosition.z
            };

            int nextAgentIndex = board->getIndex(nextAgentPosition);

#pragma region UPDATE_STAIR_POSITION
            if (DIR_X[idir] == 0 and DIR_Y[idir] == 0)
            {
                if (board->isTile(nextAgentPosition, STAIR_UP))
                {
                    nextAgentPosition.z += 1;
                }
                else if (board->isTile(nextAgentPosition, STAIR_DOWN))
                {
                    nextAgentPosition.z -= 1;
                }
            }
#pragma endregion

#pragma region UPDATE_KEY_MASK
            if (board->isTile(nextAgentPosition, KEY))
            {
                nextAgentState.keyMask |= GET_MASK(board->getBoardValue(nextAgentPosition));
            }
#pragma endregion

            
#pragma region CHECK_TILE_VALID_AND_UPD_BOOLEAN
            if (board->getIndex(nextAgentPosition) == -1) continue;
            if (board->isTile(nextAgentPosition, OBSTACLE)) continue;

            int tileValue = board->getBoardValue(nextAgentPosition);

            bool noDoorOrHasKey =
                not board->isTile(nextAgentPosition, DOOR) or
                VALUE_CONTAIN_MASK(tileValue, nextAgentState.keyMask);

            bool anotherAgentOccured = false;
            for (int jagent = 0; jagent < MAX_AGENT_COUNT; jagent++)
            {
                if (iagent != jagent && nextAgentIndex == statePtr->agents[jagent].index)
                {
                    anotherAgentOccured = true;
                    break;
                }
            }

            bool isTileValid = noDoorOrHasKey and not anotherAgentOccured;
#pragma endregion

            if (isTileValid)
            {
                nextAgentState.index = board->getIndex(nextAgentPosition); // update new state
                if (nextAgentState.desiredTargets.size() > 0 && 
                    nextAgentState.desiredTargets.back() == nextAgentIndex) //if arrive at a desire target
                {
                    nextAgentState.desiredTargets.pop_back();
                    if (nextAgentState.desiredTargets.size() == 0) // desire target is a target
                    { 
                        nextAgentState.point += 1;// gain point

                        if (iagent != 0)
                        {
                            uint seed = 0;
                            hash_combine(seed, nextAgentState.index);
                            hash_combine(seed, nextAgentState.point);
                            hash_combine(seed, iagent);
                            seed = generate_seed(seed);

                            nextAgentState.desiredTargets.push_back(
                                boardData.generate_random_target_index(seed, nextAgentPosition.z));
                        }
                    }
                }
            }
            else if (board->isTile(nextAgentPosition, DOOR) and not noDoorOrHasKey) //Step on door tile but have no key for the door
            {
                int requiredKey = GET_MASK_INDEX(tileValue);
                int keyIndex = boardData.keyIndexList[requiredKey];

                if (COUNT(nextAgentState.desiredTargets, nextAgentIndex) == 0) //Add door to desire target
                {
                    nextAgentState.desiredTargets.push_back(nextAgentIndex);
                }

                if (COUNT(nextAgentState.desiredTargets, keyIndex) == 0)
                {
                    nextAgentState.desiredTargets.push_back(keyIndex); // Add key to desire target
                }
            }
            else
            {
                continue;
            }

            openedList.push(nextStatePtr);
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_timer);
    resultData.timeElapsedInMiniSeconds = duration.count();
    return isPathFound;
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

    while (fscanf_s(inputFile, "\n[%[^]]]", buffer, sizeof(buffer)))
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

    bool isSearchSuccess = search(board, resultData);

    NEW_PRINT_SECTION(RESULT)
    if (isSearchSuccess)
    {
        printf("\npath go:");
        resultData.print_path();
        //printPathTrace(*board, resultData.stateData, resultData.finalState);

        resultData.printResult();
    }
    else
    {
        printf("\nno solution found");
    }
    
}