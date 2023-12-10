#include "GuiBoard.h"

GuiBoard::GuiBoard(const shared_ptr<Board> board)
{
	this->board = make_shared<Board>(board->dim.x, board->dim.y, board->dim.z); // Should create a new board with same dimension
	this->board->gridData = board->gridData;



	nRows = board->dim.x;
	nCols = board->dim.y;
	nFloors = board->dim.z;

	truncateBoard();
}

void GuiBoard::truncateBoard()
{
	int floor_cut = 0;
	for (int k = 0; k < nFloors; k++)
	{
		int checksum = 0;
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				int value = board->getBoardValue(Position({ i, j, k }));
				checksum += value;
			}
		}
		if (checksum < 0)
		{
			floor_cut = k;
			break;
		}
	}
	nFloors = floor_cut;
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

void GuiBoard::drawUi(sf::RenderWindow& window)
{
	// Button says Next
	ImGui::SetNextWindowPos(ImVec2(window.getSize().x - ImGui::GetWindowWidth(), 0));
	ImGui::Begin("Toolbox", nullptr);
	ImGui::Text("Scroll to zoom");
	ImGui::Text("Use arrow keys to move");

	// This toolbox shall do these tasks:
	// 1. Change current state
	// 2. Show current time
	// 3. Show log

	if (isSearched)
	{
		if (isSolutionFound)
		{
			// Change current state
			if (ImGui::SliderInt("State", &stateListIterator, 0, stateList.size()))
			{
				// IMO, this is safer than the button below
				updateBoard();
			}

			ImGui::Text("Solution found");
			ImGui::Text("Slide the bar to change state");
			score = 100 - resultData.getPathCost();
			ImGui::Text("Path Cost score: %d", score);

			if (ImGui::Button("To Next State"))
			{
				if (stateListIterator < stateList.size())
				{
					// Check if reached end. Then not showing button
					stateListIterator++;
					updateBoard();
				}
			}
			if (ImGui::Button("To Previous State"))
			{
				if (stateListIterator > 0)
				{
					stateListIterator--;
					updateBoard();
				}
			}

			if (ImGui::Button("Generate Heatmap File"))
			{
				isHeatmapButtonClicked = true;
				generateHeatMap();
			}
			if (isHeatmapButtonClicked)
			{
				ImGui::Text("Heatmap file generated");
			}
		}
		else
		{
			ImGui::Text("No solution found");
		}
	}
	else
	{
		// Mannn, I don't want to do this better
		shared_ptr<Board> tmp_board = make_shared<Board>(this->board->dim.x, this->board->dim.y, this->board->dim.z);
		tmp_board->gridData = this->board->gridData;

		if (ImGui::Button("BCS/UCS Search"))
		{
			bool isSearchSuccess = algorithm::search(tmp_board, resultData);
			algorithm::use_heuristic = false;

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

			isSearched = true;
			isSolutionFound = isSearchSuccess;

			stateList = resultData.get_path();
			stateListIterator = 0;
		}

		if (ImGui::Button("A* Search"))
		{
			bool isSearchSuccess = algorithm::search(tmp_board, resultData);
			algorithm::use_heuristic = true;

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

			isSearched = true;
			isSolutionFound = isSearchSuccess;

			stateList = resultData.get_path();
			stateListIterator = 0;
		}
	}

	ImGui::End();
	ImGui::SFML::Render(window);
}

void GuiBoard::drawBoard(sf::RenderWindow& window)
{
	// Get the max number of board in a row. This is used to arrange floors in square formation
	int nGridWidth = ceil(sqrt(nFloors));

	// This dynamic thing is not working for big text size
	// mapSize = min(window.getView().getSize().x, window.getView().getSize().y) / nGridWidth;
	// cellSize = mapSize / max(nRows, nCols);
	// textSize = cellSize / 2;
	// cellMargin = cellSize / 2;
	// textMargin = cellSize / 10;
	// cellBorder = cellSize / 20;

	int indexNo = 0;

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

				// Draw index
				string indexText = to_string(indexNo);

				sf::Text index;
				index.setFont(font);
				index.setCharacterSize(15);
				index.setFillColor(sf::Color(23, 23, 23));
				index.setStyle(sf::Text::Bold);
				index.setPosition(pos_x, pos_y);
				index.setString(indexText);
				window.draw(index);
				indexNo++;

			}
		}
	}
}

int GuiBoard::drawHeatMap()
{
	const char* pythonScript = "heatmap.py -heat ./heatmap.txt";
	std::string command = "python " + std::string(pythonScript);
	int result = system(command.c_str());

	// Check the result of the command execution
	if (result == 0) {
		// Command executed successfully
		return 0;
	}
	else {
		// An error occurred
		return 1;
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
		// for (int j = 0; j < visitedLocation.size(); j++)
		// {
		// 	if (visitedLocation[j] == i)
		// 	{
		// 		if (stateListIterator > visitedState[j])
		// 		{
		// 			this->board->gridData[i] = 0;
		// 			break;
		// 		}
		// 		else
		// 		{
		// 			visitedState.erase(visitedState.begin() + j);
		// 			visitedLocation.erase(visitedLocation.begin() + j);
		// 			break;
		// 		}
		// 	}
		// }
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
		// int value = this->board->getBoardValue(newAgentPosList[i]);
		// switch (value & 0xF)
		// {
		// case KEY:
		// {
		// 	visitedState.push_back(stateListIterator);
		// 	int gridIndex = this->board->getIndex(newAgentPosList[i]);
		// 	visitedLocation.push_back(gridIndex);
		// 	break;
		// }
		// case DOOR:
		// {
		// 	visitedState.push_back(stateListIterator);
		// 	int gridIndex = this->board->getIndex(newAgentPosList[i]);
		// 	visitedLocation.push_back(gridIndex);
		// 	break;
		// }
		// case TARGET:
		// {
		// 	visitedState.push_back(stateListIterator);
		// 	int gridIndex = this->board->getIndex(newAgentPosList[i]);
		// 	visitedLocation.push_back(gridIndex);
		// 	break;
		// }
		// }

		char tmp = char('1' + i);
		char c[3] = { 'A', tmp , '\0' };
		this->board->setBoardData(newAgentPosList[i], c);
	}
}

void GuiBoard::generateHeatMap()
{
	vector<int> heatMap = vector<int>(board->gridData.size(), 0);
	for (auto state : stateList)
	{
		for (int i = 0; i < MAX_AGENT_COUNT; i++)
		{
			if (state.agents[i].index != -1)
			{
				heatMap[state.agents[i].index]++;
			}
		}
	}

	for (int k = 0; k < nFloors; k++)
	{
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				int value = board->getBoardValue(Position({ i, j, k }));

				if (value == OBSTACLE)
				{
					heatMap[board->getIndex(Position({ i, j, k }))] = -1;
				}
			}
		}
	}

	// Write to file
	FILE* outputFile;
	if (fopen_s(&outputFile, "./heatmap.txt", "w") != 0)
	{
		printf("Cannot open file");
		return;
	}

	fprintf(outputFile, "%d, %d\n", nRows, nCols);
	for (int k = 0; k < MAX_BOARD_HEIGHT; k++)
	{
		fprintf(outputFile, "[floor%d]\n", k + 1);
		for (int i = 0; i < nRows; i++)
		{
			for (int j = 0; j < nCols; j++)
			{
				int value = heatMap[board->getIndex(Position({ i, j, k }))];

				if (value > 0)
				{
					fprintf(outputFile, "%d", value);
				}
				else if (value == -1)
				{
					fprintf(outputFile, "-1");
				}
				else
				{
					fprintf(outputFile, "0");
				}
				if (j != nCols - 1)
				{
					fprintf(outputFile, ",");
				}
			}
			fprintf(outputFile, "\n");
		}
		// fprintf(outputFile, "\n");
	}
	printf("\nHeatmap generated\n");

	fclose(outputFile);
	if (drawHeatMap())
	{
		printf("Cannot export heatmap");
	}
	else
	{
		printf("\nHeatmap exported");
	}
}

void GuiBoard::run()
{
	int windowWidth = WINDOW_WIDTH;
	int windowHeight = WINDOW_HEIGHT;

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
			case sf::Event::MouseWheelScrolled:
			{
				if (event.mouseWheelScroll.delta > 0)
				{
					view.zoom(zoomView + 0.05f * event.mouseWheelScroll.delta);
					window.setView(view);
				}
				else if (event.mouseWheelScroll.delta < 0)
				{
					view.zoom(zoomView + 0.05f * event.mouseWheelScroll.delta);
					window.setView(view);
				}
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
	shared_ptr<Board> board;

	// CLI ask user for input file
	char inputFilePath[100] = DEFAULT_INPUT_PATH;

	//char inputFilePath[100];
	 //printf("Input file path: ");
	 //scanf_s("%s", inputFilePath, sizeof(inputFilePath));

	try
	{
		if (algorithm::read(board, DEFAULT_INPUT_PATH) == false)
		{
			throw "Cannot read file";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "\nError in reading file!" << e.what() << std::endl;
		std::cerr << "Caught exception: " << e.what() << std::endl;
	}

	try
	{
		GuiBoard guiBoard(board);
		guiBoard.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "\nError in running GUI!" << e.what() << std::endl;
		std::cerr << "Caught exception: " << e.what() << std::endl;
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