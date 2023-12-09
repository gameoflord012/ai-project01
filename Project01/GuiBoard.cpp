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
	this->board = make_shared<Board>(board->dim.x, board->dim.y, board->dim.z); // Should create a new board with same dimension
	this->board->gridData = board->gridData; // Copy the board data

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
	ImGui::SetNextWindowPos(ImVec2(window.getSize().x - ImGui::GetWindowWidth(), 0), ImGuiCond_Always);
	ImGui::Begin("Toolbox", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

	// This toolbox shall do these tasks:
	// 1. Change current state
	// 2. Show current time
	// 3. Show log

	// Change current state
	if (ImGui::SliderInt("State", &stateListIterator, 0, stateList.size()))
	{
		// IMO, this is safer than the button below
		updateBoard();
	}

	ImGui::Text("Solution found");
	ImGui::Text("Slide the bar to see agents location across states");

	if (ImGui::Button("To next State"))
	{
		if (stateListIterator < stateList.size())
		{
			// Check if reached end. Then not showing button
			stateListIterator++;
			updateBoard();
		}
	}
	if (ImGui::Button("To previous State"))
	{
		if (stateListIterator > 0)
		{
			stateListIterator--;
			updateBoard();
		}
	}

	ImGui::End();
	ImGui::SFML::Render(window);
}

void GuiBoard::drawBoard(sf::RenderWindow& window)
{
	// Get the max number of board in a row. This is used to arrange floors in square formation
	int nGridWidth = ceil(sqrt(nFloors));

	mapSize = min(window.getSize().x, window.getSize().y) / nGridWidth;
	cellSize = mapSize / max(nRows, nCols);
	textSize = cellSize / 2;
	cellMargin = cellSize / 2;
	textMargin = cellSize / 10;
	cellBorder = cellSize / 20;

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
				if (value == OBSTACLE)
				{
					cell.setFillColor(sf::Color(129, 129, 129));
				}

				cell.setOutlineThickness(cellBorder);
				cell.setOutlineColor(customColor_3);

				int pos_x = ((k % nGridWidth) * (cellSize * nCols + cellMargin)) + (j * cellSize);
				int pos_y = ((gridRow - 1) * (cellSize * nRows + cellMargin)) + (i * cellSize);
				cell.setPosition(pos_x, pos_y);
				window.draw(cell);

				// Draw text
				sf::Text text;
				text.setFont(font);
				text.setCharacterSize(textSize);
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
	// Copy the board from stateList because they doesn't change. Let default index be 0
	for (int i = 0; i < stateList[0].board->gridData.size(); i++)
	{
		int tmp = stateList[0].board->gridData[i];
		this->board->gridData[i] = tmp;
		// Remove visited item, or pop item from visited list
		for (int j = 0; j < visitedLocation.size(); j++)
		{
			if (visitedLocation[j] == i)
			{
				if (stateListIterator > visitedState[j])
				{
					this->board->gridData[i] = 0;
					break;
				}
				else
				{
					visitedState.erase(visitedState.begin() + j);
					visitedLocation.erase(visitedLocation.begin() + j);
					break;
				}
			}
		}
	}
	if (stateListIterator == 0)
	{
		return;
	}

	// This stores new agent position
	vector<Position> newAgentPosList;
	for (int i = 0; i < MAX_AGENT_COUNT; i++)
	{
		if (stateList[stateListIterator - 1].agents[i].index != -1)
		{
			newAgentPosList.push_back(stateList[stateListIterator - 1].board->getPosition(stateList[stateListIterator - 1].agents[i].index));
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
		int value = this->board->getBoardValue(newAgentPosList[i]);
		switch (value & 0xF)
		{
		case KEY:
		{
			visitedState.push_back(stateListIterator);
			int gridIndex = this->board->getIndex(newAgentPosList[i]);
			visitedLocation.push_back(gridIndex);
			break;
		}
		case DOOR:
		{
			visitedState.push_back(stateListIterator);
			int gridIndex = this->board->getIndex(newAgentPosList[i]);
			visitedLocation.push_back(gridIndex);
			break;
		}
		case TARGET:
		{
			visitedState.push_back(stateListIterator);
			int gridIndex = this->board->getIndex(newAgentPosList[i]);
			visitedLocation.push_back(gridIndex);
			break;
		}
		}

		char tmp = char('1' + i);
		char c[3] = { 'A', tmp , '\0' };
		this->board->setBoardData(newAgentPosList[i], c);
	}
}

void GuiBoard::run()
{
	int windowWidth = 1920;
	int windowHeight = 1080;

	float dt;
	float viewSpeed = 300.f;
	float zoomView = 1.0f;

	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Project 01 - KAT GUI");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

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

	try
	{
		if (algorithm::read(board, "../input/input1-level3.txt") == false)
		{
			throw "Cannot read file";
		}

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
	}
	catch (const char* msg)
	{
		printf("\n%s", msg);
	}
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