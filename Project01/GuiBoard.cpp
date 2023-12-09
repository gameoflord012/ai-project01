#include "GuiBoard.h"

GuiBoard::GuiBoard(const shared_ptr<Board> board)
{
	this->board = board;
	nRows = board->dim.x;
	nCols = board->dim.y;
	nFloors = board->dim.z;
}

GuiBoard::GuiBoard(const shared_ptr<Board> board, SearchResultData resultData)
{
	this->board = board;
	nRows = board->dim.x;
	nCols = board->dim.y;
	nFloors = board->dim.z;
	this->resultData = resultData;
	stateList = resultData.get_path();

	stateListIterator = 0;
}

void GuiBoard::drawHeatMap(sf::RenderWindow& window)
{
}

void GuiBoard::drawUi(sf::RenderWindow& window)
{
	// Button says Next
	ImGui::Begin("Toolbox");
	// This toolbox shall do these tasks:
	// 1. Change current state
	// 2. Show current time
	// 3. Show log

	// Change current state
	if (ImGui::SliderInt("State", &stateListIterator, 0, stateList.size() - 1))
	{
		// IMO, this is safer than the button below
		updateBoard();
	}

	if (stateListIterator < stateList.size())
	{
		// Check if reached end. Then not showing button
		if (ImGui::Button("To next State"))
		{
			updateBoard();
			stateListIterator++;
		}
	}

	// Show current time
	if (stateListIterator < stateList.size())
	{
		ImGui::Text("Time: %d", stateList[stateListIterator].time);
	}

	// Show log (WIP)



	ImGui::End();
	ImGui::SFML::Render(window);
}

void GuiBoard::drawBoard(sf::RenderWindow& window)
{
	int cellSize = 100;
	int textMargin = 10;
	int cellMargin = 20;

	// Get the max number of board in a row. This is used to arrange floors in square formation
	int nGridWidth = ceil(sqrt(nFloors));

	int gridRow = 0;
	for (int k = 0; k < nFloors; k++)
	{
		if (k % nGridWidth == 0)
		{
			gridRow++;
		}

		// Draw cells, the cells will form a board (floor)
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				// Get cell value
				int value = board->getBoardValue(Position({ i, j, k }));

				// Draw cell
				sf::RectangleShape cell(sf::Vector2f(cellSize, cellSize));
				cell.setFillColor(customColor_2);
				cell.setOutlineThickness(5);
				cell.setOutlineColor(customColor_3);

				int pos_x = ((k % nGridWidth) * (cellSize * nCols + cellMargin)) + (j * cellSize);
				int pos_y = ((gridRow - 1) * (cellSize * nRows + cellMargin)) + (i * cellSize);
				cell.setPosition(pos_x, pos_y);
				window.draw(cell);

				// Draw text
				sf::Text text;
				text.setFont(font);
				text.setCharacterSize(20);
				text.setFillColor(textColor_Wall);
				text.setStyle(sf::Text::Bold);
				text.setPosition(pos_x + textMargin, pos_y + textMargin);
				getBoardText(value, text);
				window.draw(text);
			}
		}
	}
}

void GuiBoard::updateBoard()
{
	// This stores new agent position
	vector<Position> newAgentPosList;
	for (int i = 0; i < MAX_AGENT_COUNT; i++)
	{
		if (stateList[stateListIterator].agents[i].index != -1)
		{
			newAgentPosList.push_back(stateList[stateListIterator].board->getPosition(stateList[stateListIterator].agents[i].index));
		}
	}

	// Clear all old agents positions
	for (int k = 0; k < nFloors; k++)
	{
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				// Get cell value
				int value = this->board->getBoardValue(Position({ i, j, k }));

				// Check if this cell is agent, delete it
				if ((value & 0xF) == AGENT)
				{
					this->board->setBoardData(Position({ i, j, k }), "0");
				}
			}
		}
	}

	// Set new agent positions
	for (int i = 0; i < newAgentPosList.size(); i++)
	{
		char tmp = char('1' + i );
		char c[3] = { 'A', tmp , '\0' };
		this->board->setBoardData(newAgentPosList[i], c);
	}
}

void GuiBoard::run()
{
	int windowWidth = 1920;
	int windowHeight = 1080;

	float dt;

	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Project 01 - KAT GUI");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	float viewSpeed = 300.f;
	sf::View view(sf::FloatRect(0, 0, windowWidth, windowHeight));
	view.setCenter(window.getSize().x / 2, window.getSize().y / 2);
	window.setView(view);

	font.loadFromFile("Resources/Fonts/Roboto/roboto_regular.ttf");

	while (window.isOpen())
	{
		// Delta Clock
		dt = dtClock.restart().asSeconds();

		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			switch (event.type) {
			case sf::Event::Closed:
			{
				window.close();
				break;
			}
			case sf::Event::Resized:
			{
				view.setSize(event.size.width, event.size.height);
				window.setView(view);
				break;
			}
			}
		}

		ImGui::SFML::Update(window, dtClock.restart());

		// View movement with keyboard
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			view.move(-viewSpeed * dt, 0);
			window.setView(view);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			view.move(viewSpeed * dt, 0);
			window.setView(view);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			view.move(0, -viewSpeed * dt);
			window.setView(view);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			view.move(0, viewSpeed * dt);
			window.setView(view);
		}

		// Render
		window.clear(customColor_1);

		drawBoard(window);
		drawUi(window);
		//drawHeatMap(window);

		window.display();
	}
	ImGui::SFML::Shutdown();
}

int main()
{
	SearchResultData resultData;
	shared_ptr<Board> board;

	algorithm::read(board, "../input/inp.txt");
	bool isSearchSuccess = algorithm::search(board, resultData);

	NEW_PRINT_SECTION(RESULT)
		if (isSearchSuccess)
		{
			printf("\nPath go:");
			resultData.print_path();

			resultData.printResult();
		}
		else
		{
			printf("\nNo solution found");
		}

	GuiBoard guiBoard(board, resultData);
	guiBoard.run();
	return 0;
}

void getBoardText(int value, sf::Text& text)
{
	switch (value)
	{
	case OBSTACLE:
	{
		text.setString("X");
		text.setFillColor(textColor_Wall);
		return;
	}
	case STAIR_UP:
	{
		text.setString("UP");
		text.setFillColor(textColor_Stair);
		return;
	}
	case STAIR_DOWN:
	{
		text.setString("DO");
		text.setFillColor(textColor_Stair);
		return;
	}
	default:
	{
		text.setString(" ");
		break;
	}
	}
	// Get the masked value
	int tmp = value & 0xF;
	int p = ROUND_INT(log2(value >> 4));
	switch (tmp)
	{
	case AGENT:
	{
		text.setString("A" + to_string(p + 1));
		text.setFillColor(agent_target_ColorList[p]);
		return;
	}
	case TARGET:
	{
		text.setString("T" + to_string(p + 1));
		text.setFillColor(agent_target_ColorList[p]);
		return;
	}
	case KEY:
	{
		text.setString("K" + to_string(p + 1));
		text.setFillColor(key_door_ColorList[p]);
		return;
	}
	case DOOR:
	{
		text.setString("D" + to_string(p + 1));
		text.setFillColor(key_door_ColorList[p]);
		return;
	}
	default:
	{
		text.setString(" ");
		return;
	}
	}
	return;
}

void generateHeatMap(const vector<SearchState>& stateList)
{
}