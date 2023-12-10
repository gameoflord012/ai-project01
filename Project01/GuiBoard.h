#pragma once

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>

#include "SFML/Graphics.hpp"
#include "imgui.h"
#include "imgui-SFML.h"

#include "Config.h"
#include "Position.h"
#include "Board.h"
#include "SearchState.h"
#include "SmartPtr.h"
#include "Search.h"
#include "Helpers.h"

const sf::Color customColor_1 = sf::Color(121, 121, 210); // Background
const sf::Color customColor_2 = sf::Color(166, 211, 226); // Cell color
const sf::Color customColor_3 = sf::Color(237, 133, 84); // Border color
const sf::Color textColor_Stair = sf::Color(102, 0, 255); // Text color 1
const sf::Color textColor_Wall = sf::Color(178, 178, 178); // Text color 5

// list of colors for agent/target
const sf::Color agent_target_ColorList[MAX_AGENT_COUNT] = {
	sf::Color(153, 0, 0),
	sf::Color(0, 255, 0),
	sf::Color(0, 0, 255),
	sf::Color(255, 255, 0),
	sf::Color(255, 0, 255),
	sf::Color(0, 255, 255),
	sf::Color(255, 255, 255),
	sf::Color(128, 0, 0),
	sf::Color(0, 128, 0),
};

// list of colors for key/door
const sf::Color key_door_ColorList[MAX_KEY_COUNT] = {
	sf::Color(102, 0, 204),
	sf::Color(204, 41, 0),
	sf::Color(0, 0, 255),
	sf::Color(255, 255, 0),
	sf::Color(255, 0, 255),
	sf::Color(0, 255, 255),
	sf::Color(255, 255, 255),
	sf::Color(128, 0, 0),
	sf::Color(0, 128, 0),
};

using namespace std;

// this class is used to draw the board, update board according to agent movement, and show heatmap
class GuiBoard
{
public:
	GuiBoard(const shared_ptr<Board> board);
	GuiBoard(const shared_ptr<Board> board, SearchResultData resultData);

	void drawBoard(sf::RenderWindow& window);
	void drawHeatMap(sf::RenderWindow& window);
	void drawUi(sf::RenderWindow& window);

	void updateBoard();

	void generateHeatMap();


	void run();

	bool isSolutionFound = false;
	bool isSearched = false;


private:
	shared_ptr<Board> board;
	StatePtr statePtr;

	int nRows;
	int nCols;
	int nFloors;
	int stateListIterator; // This is integer because I dont want to use pointer

	int mapSize; // Map holds board that holds cells
	int textSize = 20;
	int cellSize = 100;
	int textMargin = 10;
	int cellMargin = 20;
	int cellBorder = 5;

	bool isHeatmapButtonClicked = false;


	SearchResultData resultData;
	vector<SearchState> stateList;

	//vector <int> visitedState;
	//vector<int> visitedLocation;

	sf::RenderWindow window;
	sf::Sprite map_sprite;
	sf::Texture font_texture;
	sf::Texture map_texture;
	sf::Font font;
	sf::Clock dtClock;
};

// Helpers functions

// Decrypt board value to text (Ex: 17 -> "A1")
void getBoardText(int value, sf::Text& text);

// Generate heatmap, depicts the number of agents that have passed through each cell
