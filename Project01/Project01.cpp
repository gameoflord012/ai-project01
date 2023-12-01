#include "SFML/Graphics.hpp"
#include <SFML/Graphics/Font.hpp>

#include <iostream>
#include <vector>
#include <stdio.h>
#include <cassert>
#include <memory>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <chrono>

#define MAX_BOARD_HEIGHT 10
#define MAX_AGENT_COUNT 9

#include "Position.h"
#include "Board.h"
#include "SearchState.h"

using namespace std;

const int minCellSize = 20;
int CELL_SIZE = 30;
sf::Font font;
const std::vector<sf::Color> colorList = {
	sf::Color::Red,
	sf::Color::Green,
	sf::Color::Blue,
	sf::Color::Yellow,
	sf::Color::Magenta,
	sf::Color::Cyan,
	sf::Color::White,
	sf::Color::Black,
};

template <typename T>
using MinHeap = std::priority_queue<T, std::vector<T>, std::greater<T>>;
typedef pair<int, int> HeapType;
typedef MinHeap<HeapType> SearchHeap;

bool search(const Board& board, SearchResultData& resultData);
void printPathTrace(const Board& board, const vector<SearchState>& stateData, const SearchState& traceState);

void renderBoard(sf::RenderWindow& window, Board board);
void initializeFont();
void getBoardText(int value, sf::Text& text);

int main()
{
	shared_ptr<Board> board;

#pragma region READ_BOARD
	int nCols, nRows;
	FILE* inputFile;

	// Hard-code input file path
	if (fopen_s(&inputFile, "../input/inp.txt", "r") != 0)
	{
		printf("File not found");
		return -1;
	}

	wrap_fscanf_s(inputFile, "%d,", &nRows);
	wrap_fscanf_s(inputFile, "%d", &nCols);

	board = make_shared<Board>(nRows, nCols);

	char buffer[50];
	int curBoardZ = 0;

	NEW_PRINT_SECTION(INPUT);

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
#pragma region PERFORM_SEARCH
	SearchResultData resultData;

	bool isSearchSuccess = search(*board, resultData);

	NEW_PRINT_SECTION(RESULT)
		if (isSearchSuccess)
		{
			printf("\nPath go:");
			printPathTrace(*board, resultData.stateData, resultData.finalState);

			resultData.printResult();
		}
		else
		{
			printf("\nNo solution found");
		}
#pragma endregion

	// ================================================= Kat section ========================================

	// SFML render
	sf::RenderWindow window(sf::VideoMode(800, 600), "Game Board");
	sf::View view(sf::Vector2f(350.f, 300.f), sf::Vector2f(300.f, 200.f));
	window.setView(view);

	// Get the current window size
	int windowWidth = window.getSize().x;
	int windowHeight = window.getSize().y;

	// Tmp, will fix (WIP)
	int nFloors = 1;
	// Cell size, only works with side-by-side floor
	CELL_SIZE = windowWidth / nFloors / (nCols + 1);

	initializeFont();

	bool isDragging = false;
	sf::Vector2f lastMousePosition;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::Resized:
				// Resize the view to the new size of the window
				view.setSize(event.size.width, event.size.height);
				window.setView(view);
				break;
			case sf::Event::MouseWheelScrolled:
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
					if (event.mouseWheelScroll.delta > 0) {
						view.zoom(0.9f);
					}
					else if (event.mouseWheelScroll.delta < 0) {
						view.zoom(1.1f);
					}
					window.setView(view);
				}
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Up) {
					view.move(0, -10);
					window.setView(view);
				}
				else if (event.key.code == sf::Keyboard::Down) {
					view.move(0, 10);
					window.setView(view);
				}
				else if (event.key.code == sf::Keyboard::Left) {
					view.move(-10, 0);
					window.setView(view);
				}
				else if (event.key.code == sf::Keyboard::Right) {
					view.move(10, 0);
					window.setView(view);
				}
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left) {
					isDragging = true;
					lastMousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				}
				break;
			case sf::Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left) {
					isDragging = false;
				}
				break;
			case sf::Event::MouseMoved:
				if (isDragging) {
					sf::Vector2f newMousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
					sf::Vector2f delta = lastMousePosition - newMousePosition;
					view.move(delta);
					window.setView(view);
					lastMousePosition = newMousePosition;
				}
				break;
			default:
				break;
			}
		}

		window.clear(sf::Color::White);

		renderBoard(window, *board);

		window.display();
	}

	return 0;
}

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

				bool anotherAgentOccured = false;
				for (int jagent = 0; jagent < MAX_AGENT_COUNT; jagent++)
				{
					if (iagent != jagent && nextAgentIndex == state.agentIndexes[jagent])
					{
						anotherAgentOccured = true;
						break;
					}
				}

				bool isTileValid = noObstacle and noDoorOrHasKey and not anotherAgentOccured;

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

	for (int i = 0; i < MAX_AGENT_COUNT; i++) if (traceState.agentIndexes[i] != -1)
	{
		Position p = board.getPosition(traceState.agentIndexes[i]);
		printf("\n[A%1d] Floor %2d, ROW = %3d, COL = %3d", i + 1, p.z, p.x, p.y);
	}
}

void renderBoard(sf::RenderWindow& window, Board board)
{
	int nRows = board.dim.x;
	int nCols = board.dim.y;

	// Temp fix, need to fix nFloors
	//int nFloors = board.gridData.size() / (nRows * nCols);
	int nFloors = 3;

	int cellSize = CELL_SIZE;
	int cellMargin = 5;
	int floorMargin = 10;

	// For now, floor display side-by-side
	int boardWidth = nFloors * (nCols * cellSize + (nCols + 1) * cellMargin) + (nFloors - 1) * floorMargin;
	int boardHeight = nRows * (cellSize + cellMargin);

	for (int i_floor = 0; i_floor < nFloors; i_floor++)
	{
		for (int i_row = 0; i_row < nRows; i_row++)
		{
			for (int i_col = 0; i_col < nCols; i_col++)
			{
				// Draw tile
				sf::RectangleShape tile(sf::Vector2f(cellSize, cellSize));

				tile.setFillColor(sf::Color::White);
				tile.setOutlineThickness(1);
				tile.setOutlineColor(sf::Color::Black);

				tile.setPosition(
					i_floor * (nCols * cellSize + (nCols + 1) * cellMargin) + i_col * (cellSize + cellMargin) + floorMargin,
					i_row * (cellSize + cellMargin)
				);

				// Get tile value
				int tileValue = board.getBoardValue(Position({ i_row, i_col, i_floor }));

				// Draw text inside tile
				sf::Text text;
				int fontSize = cellSize / 2;
				getBoardText(tileValue, text);
				text.setFont(font);
				text.setCharacterSize(fontSize);
				text.setFillColor(sf::Color::Red);
				text.setStyle(sf::Text::Bold);
				text.setPosition(tile.getPosition().x + 5, tile.getPosition().y + 5);

				window.draw(tile);
				window.draw(text);
			}
		}
	}
}

void initializeFont() {
	if (!font.loadFromFile("./Fonts/Roboto/roboto_regular.ttf")) {
		std::cerr << "Failed to load font." << std::endl;
	}
}

void getBoardText(int value, sf::Text& text)
{
	// Check if value is a special tile, because it's not masked
	switch (value)
	{
	case OBSTACLE:
		text.setString("X");
		return;
	case STAIR_UP:
		text.setString("UP");
		return;
	case STAIR_DOWN:
		text.setString("DO");
		return;
	default:
		text.setString(" ");
		break;
	}

	// Get the masked value
	int tmp = value & 0xF;
	int p = ROUND_INT(log2(value >> 4));
	switch (tmp)
	{
	case AGENT:
		text.setString("A" + to_string(p + 1));
		text.setFillColor(colorList[p]);
		return;
	case TARGET:
		text.setString("T" + to_string(p + 1));
		return;
	case KEY:
		text.setString("K" + to_string(p + 1));
		return;
	case DOOR:
		text.setString("D" + to_string(p + 1));
		return;
	default:
		text.setString(" ");
		return;
	}
	return;
}