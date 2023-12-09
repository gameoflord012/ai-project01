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

const sf::Color customColor_1 = sf::Color(95, 35, 107); // Background
const sf::Color customColor_2 = sf::Color(190, 55, 95); // Cell color
const sf::Color customColor_3 = sf::Color(237, 133, 84); // Border color
const sf::Color textColor_Stair = sf::Color(255, 212, 229); // Text color 1
const sf::Color textColor_Wall = sf::Color(186, 255, 255); // Text color 5

// list of colors for agent/target
const sf::Color agent_target_ColorList[MAX_AGENT_COUNT] = {
	sf::Color(255, 0, 0),
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
	sf::Color(255, 0, 0),
	sf::Color(0, 255, 0),
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

	void run();

private:
	shared_ptr<Board> board;
	StatePtr statePtr;

	int nRows;
	int nCols;
	int nFloors;
	int stateListIterator; // This is integer because I dont want to use pointer	

	SearchResultData resultData;
	vector<SearchState> stateList;


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
void generateHeatMap(const vector<SearchState>& stateList);
