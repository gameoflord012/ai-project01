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

const sf::Color customColor_1 = sf::Color(95, 35, 107); // Background
const sf::Color customColor_2 = sf::Color(190, 55, 95); // Cell color
const sf::Color customColor_3 = sf::Color(237, 133, 84); // Border color
const sf::Color textColor_Stair = sf::Color(255, 212, 229); // Text color 1
const sf::Color textColor_Agent = sf::Color(186, 255, 201); // Text color 2
const sf::Color textColor_Key = sf::Color(241, 188, 255); // Text color 3
const sf::Color textColor_Door = sf::Color(254, 255, 163); // Text color 4
const sf::Color textColor_Wall = sf::Color(186, 255, 255); // Text color 5

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
	int nRows;
	int nCols;
	int nFloors;
	SearchResultData resultData;
	sf::RenderWindow window;
	sf::Sprite map_sprite;
	sf::Texture font_texture;
	sf::Texture map_texture;
	sf::Font font;
	sf::Clock dtClock;

};

void getBoardText(int value, sf::Text& text);