#include<iostream>
#include "Game.h"
#include <SFML/Graphics.hpp>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment (lib, "Ws2_32.lib")

Game::Game()
{
	if (!backgroundTexture.loadFromFile("background.png")) {
		std::cerr << "B³¹d ³adowania background.png" << std::endl;
	}
	if (!texture1.loadFromFile("cross.png")) {
		std::cerr << "B³¹d ³adowania background.png" << std::endl;
	}
	if (!texture2.loadFromFile("naught.png")) {
		std::cerr << "B³¹d ³adowania background.png" << std::endl;
	}
	char temp[BOARD_SIZE][BOARD_SIZE] = { {'1','2','3'},{'4','5','6'},{'7','8','9'} };
	x = 9;
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			board[i][j] = temp[i][j];
		}
	}
}
void Game::draw(sf::RenderWindow& window) {
	sf::Sprite sprite(backgroundTexture);
	window.draw(sprite);
	float width = window.getSize().x;
	float height = window.getSize().y;
	float cellSize = width / 3.0f;
	sf::RectangleShape line(sf::Vector2f(width, 5));

	line.setFillColor(sf::Color::Black);
	for (int i = 0; i < 3; ++i) {
		line.setPosition({ 0, i * cellSize });
		window.draw(line);
	}
	line.setSize(sf::Vector2f(height, 5));
	line.setRotation(sf::degrees(90));

	for (int i = 1; i < 3; ++i) {
		line.setPosition({ i * cellSize, 0 });
		window.draw(line);
	}
	sf::Font font("ARIAL.ttf");
	sf::Texture texture1("cross.png");
	sf::Texture texture2("naught.png");
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 'X') {
				sf::Sprite sprite1(texture1);
				sprite1.setScale({ 0.3f, 0.3f });
				//640px;
				sprite1.setPosition({ j * cellSize + 4, i * cellSize + 4 });
				window.draw(sprite1);
			}
			if (board[i][j] == 'O')
			{
				sf::Sprite sprite2(texture2);
				sprite2.setScale({ 0.3f,0.3f });
				sprite2.setPosition({ j * cellSize + 4, i * cellSize + 4 });
				window.draw(sprite2);
			}
		}
	}
}

void Game::Display_board()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			std::cout << this->board[i][j]<<" ";
		}
		std::cout << std::endl;
	}
}
bool Game::is_Win()
{
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		if (board[i][0] == board[i][1] && board[i][1]==board[i][2])
		{
			return true;
		}
		if (board[0][i] == board[1][i] && board[1][i] == board[2][i])
		{
			return true;
		}
		if (board[0][0] == board[1][1] && board[1][1] == board[2][2] || board[0][2] == board[1][1] && board[1][1] == board[2][0])
		{
			return true;
		}
	}
	return false;
}
bool Game::is_Draw()
{

	if (is_Win() == true)
	{
		return false;
	}
	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] != 'X' && board[i][j] != 'O')
			{
				return false;
			}
		}
	}
	return true;
	
}
void Game::move(int player,bool &war, sf::RenderWindow& window,SOCKET ClientSocket)
{
	int move,row,col,x,y;
		sf::Vector2i mousePos = sf::Mouse::getPosition(window);
		float cellSize = window.getSize().x / 3.0f;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
		{
			window.close();
		}
		if (player == 2)
		{
			
			row = mousePos.y / cellSize;
			col = mousePos.x / cellSize;
			while (mousePos.x < 0 || mousePos.x >= 3 * cellSize || mousePos.y < 0 || mousePos.y >= 3 * cellSize)
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
				{
					window.close();
					break;
				}
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					mousePos = sf::Mouse::getPosition(window);
					row = mousePos.y / cellSize;
					col = mousePos.x / cellSize;
				}
			}
		}
		else
		{
			receiveMove(ClientSocket, row, col);
		}
		bool czy = false;
		while (czy == false)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				window.close();
				break;
			}
			if (this->is_Legal(row, col) == true)
			{
				if (player == 2)
				{
					board[row][col] = 'X';
					sendMove(ClientSocket, row, col);
				}
				else
				{
					board[row][col] = 'O';
				}
				czy = true;
			}
			else
			{
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					mousePos = sf::Mouse::getPosition(window);
					row = mousePos.y / cellSize;
					col = mousePos.x / cellSize;
				}
			}

		}


		if ((is_Draw() == true) || (is_Win() == true))
		{
			war = false;
			std::cout << "Koniec" << std::endl;
			if (is_Win() == true)
			{
				window.clear();
				draw(window);
				window.display();
				if (player == 2)
				{
					showWinPopup();
				}
				else {
					showLosePopup();

				}
			}
			if (is_Draw() == true)
			{
				window.clear();
				draw(window);
				window.display();
				showDrawPopup();
			}

		}
		window.clear();
		draw(window);
		window.display();
}
void Game::sendMove(SOCKET clientSocket, int row, int col) {
	char move[2] = { (char)(row + '0'), (char)(col + '0') };
	send(clientSocket, move, sizeof(move), 0);
}
void Game::receiveMove(SOCKET clientSocket, int& row, int& col) {
	char move[2];
	recv(clientSocket, move, sizeof(move), 0);
	row = move[0] - '0';
	col = move[1] - '0';
}

void Game::showWinPopup() {
	sf::RenderWindow window2(sf::VideoMode({ 300, 40 }), "Wygrana!", sf::Style::None);

	sf::Font font2("ARIAL.ttf");
	sf::Text text2(font2);
	text2.setString("Wygrana[ENTER]");
	text2.setCharacterSize(30);
	text2.setFillColor(sf::Color::White);
	while (window2.isOpen()) {
		while (const std::optional event = window2.pollEvent())
		{

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
				window2.close();
		}
		window2.clear(sf::Color::Black);
		window2.draw(text2);
		window2.display();
		window2.requestFocus();
	}

}
void Game::showDrawPopup() {
	sf::RenderWindow window3(sf::VideoMode({ 300, 40 }), "Draw!", sf::Style::None);

	sf::Font font3("ARIAL.ttf");
	sf::Text text3(font3);
	text3.setString("Remis[ENTER]");
	text3.setCharacterSize(30);
	text3.setFillColor(sf::Color::White);
	while (window3.isOpen()) {
		while (const std::optional event = window3.pollEvent())
		{

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
				window3.close();
		}
		window3.clear(sf::Color::Black);
		window3.draw(text3);
		window3.display();
		window3.requestFocus();
	}



}
void Game::showLosePopup() {
	sf::RenderWindow window1(sf::VideoMode({ 300, 40 }), "Przegrana!", sf::Style::None);

	sf::Font font1("ARIAL.ttf");
	sf::Text text1(font1);
	text1.setString("Przegrana[ENTER]");
	text1.setCharacterSize(30);
	text1.setFillColor(sf::Color::White);
	while (window1.isOpen()) {
		while (const std::optional event = window1.pollEvent())
		{

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter))
				window1.close();
		}
		window1.clear(sf::Color::Black);
		window1.draw(text1);
		window1.display();
		window1.requestFocus();
	}



}

bool Game::is_Legal(int row, int col)
{
	if ((row >= 0 && row <= 2) && (col >= 0 && col <= 2))
	{
		if (board[row][col] != 'X' && board[row][col] != 'O')
		{
			return true;
		}
			
	}
	else {
		return false;
	}
}