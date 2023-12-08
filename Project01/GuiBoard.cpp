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
	// - Change color (background, table)

	if (stateListIterator < stateList.size())
	{
		// Check if reached end. Then not showing button
		if (ImGui::Button("To the next state"))
		{
			// Update board to another board
			this->board = stateList[stateListIterator].board;
			//drawBoard(window);
			stateListIterator++;
		}
	}
	ImGui::End();
	ImGui::SFML::Render(window);
}

void GuiBoard::drawBoard(sf::RenderWindow& window)
{
	int cellSize = 100;

	int textMargin = 10;
	int cellMargin = 20;

	// Get the number of board in a row.
	int nGridWidth = ceil(sqrt(nFloors));

	int gridRow = 0;
	for (int k = 0; k < nFloors; k++)
	{
		if (k % nGridWidth == 0)
		{
			gridRow++;
		}

		// Draw cells
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
}

void GuiBoard::run()
{
	float dt;

	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(1000, 800), "Project 01 - KAT GUI");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);

	float viewSpeed = 300.f;
	sf::View view(sf::FloatRect(0, 0, 800, 600));
	view.setCenter(window.getSize().x / 2, window.getSize().y / 2);

	font.loadFromFile("Resources/Fonts/Roboto/roboto_regular.ttf");

	// Init board

	while (window.isOpen())
	{
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
				window.setView(view);
				view.setSize(event.size.width, event.size.height);
				window.setView(view);
				break;
			}
			}
		}

		ImGui::SFML::Update(window, dtClock.restart());

		// Update
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
		text.setFillColor(textColor_Agent);
		return;
	}
	case TARGET:
	{
		text.setString("T" + to_string(p + 1));
		text.setFillColor(textColor_Agent);
		return;
	}
	case KEY:
	{
		text.setString("K" + to_string(p + 1));
		text.setFillColor(textColor_Key);
		return;
	}
	case DOOR:
	{
		text.setString("D" + to_string(p + 1));
		text.setFillColor(textColor_Door);
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