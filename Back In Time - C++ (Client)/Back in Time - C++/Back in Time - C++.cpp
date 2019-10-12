// Includes

#include "pch.h"
#include <iostream>
#include <string>
#include <ostream>
#include <istream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

// for convenience
using json = nlohmann::json;

const int windowWidth = 800;
const int windowHeight = 600;
const std::string title = "Back in Time";
sf::TcpSocket socket;

struct playerClass {
public:
	std::string name;
	bool onGround = false;
	float xvel = 0;
	float yvel = 0;
	float xpos = 0;
	float ypos = 0;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	sf::Sprite image;

	playerClass() {
		sf::Texture spriteTexture;
		spriteTexture.loadFromFile("data/images/dragon_knight.png");
		sf::Sprite spriteObj(spriteTexture);

		image = spriteObj;
	};
};

std::vector<playerClass> players;

sf::Packet& operator <<(sf::Packet& packet, const playerClass& player)
{
	return packet << player.name << player.xpos << player.ypos << player.xvel << player.yvel << player.up << player.down << player.left << player.right;
}

sf::Packet& operator >>(sf::Packet& packet, playerClass& player)
{
	return packet >> player.name >> player.xpos >> player.ypos >> player.xvel >> player.yvel >> player.up >> player.down >> player.left >> player.right;
}

void sendCharacter(playerClass player) {
	sf::Packet packet;
	
	packet << player;

	socket.send(packet);
}

void addPlayers(sf::Packet packet) {
	std::cout << packet << "\n";
}

void sendPlayers(std::vector<playerClass> players) {

	sf::Packet packet;

	for (int x1 = 0; x1 < players.size(); ++x1) {
		packet << players.at(x1);
	}

	socket.send(packet);
}

bool checkPlayer(playerClass player) {
	for (int x1 = 0; x1 < players.size(); ++x1) {
		if (players.at(x1).name == player.name) {
			players.at(x1).xpos = player.xpos;
			players.at(x1).ypos = player.ypos;
			players.at(x1).xvel = player.xvel;
			players.at(x1).yvel = player.yvel;
			return true;
		}
	}

	return false;
};

int main()
{
	sf::Socket::Status status = socket.connect("25.87.188.38", 53000);
	if (status != sf::Socket::Done)
	{
		// Error
	}

	socket.setBlocking(false);

	std::string username;
	std::cout << "Choose username: ";
	std::cin >> username;
	std::cout << "Choosen username: " << username << "\n";

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), title, sf::Style::Titlebar | sf::Style::Close);

	playerClass main;
	main.name = username;

	sendCharacter(main);

	sf::Packet packet2;

	addPlayers(packet2);

	//Main Loop:
	while (window.isOpen()) {
		sf::Event event;

		//Event Loop:
		while (window.pollEvent(event)) {
			switch (event.type) {

			case sf::Event::Closed:
				window.close();
			}

		}

		window.clear();

		sf::Packet playerPacket;

		for (int x = 0; x < players.size(); ++x) {
			if (players.at(x).name == username) {
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) players.at(x).up = true;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) players.at(x).down = true;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) players.at(x).left = true;;
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) players.at(x).right = true;

				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) players.at(x).up = false;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) players.at(x).down = false;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) players.at(x).left = false;
				if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) players.at(x).right = false;

				if(players.at(x).up) {
					players.at(x).yvel = -5;
				}

				if (players.at(x).down) {
					players.at(x).yvel = 5;
				}

				if (players.at(x).left) {
					players.at(x).xvel = -5;
				}

				if (players.at(x).right) {
					players.at(x).xvel = 5;
				}

				if (!players.at(x).up && !players.at(x).down) {
					players.at(x).yvel = 0;
				}

				if (!players.at(x).left && !players.at(x).right) {
					players.at(x).xvel = 0;
				}
				
				players.at(x).xpos += players.at(x).xvel;
				players.at(x).ypos += players.at(x).yvel;
			}

		}

		for (auto x = 0u; x < players.size(); x++) {
			window.draw(players.at(x).image);
			players.at(x).image.setPosition(players.at(x).xpos, players.at(x).ypos);

			players.at(x).image.move(players.at(x).xvel, players.at(x).yvel);
			if (players.at(x).name == username) {
				sendCharacter(players.at(x));
			}
		};

		sendPlayers(players);

		while (socket.receive(playerPacket) == sf::Socket::Done) {
			std::cout << "Socket received \n";
			std::string name;
			int xpos;
			int ypos;
			int xvel;
			int yvel;
			bool up;
			bool down;
			bool left;
			bool right;

			if (playerPacket >> name >> xpos >> ypos >> xvel >> yvel >> up >> down >> left >> right) {
				playerClass player;
				player.name = name;
				player.xpos = xpos;
				player.ypos = ypos;
				player.xvel = xvel;
				player.yvel = yvel;
				player.up = up;
				player.down = down;
				player.left = left;
				player.right = right;

				if (!checkPlayer(player)) {
					players.push_back(player);
					std::cout << "Player |" << player.name << "| added to players. \n";
					for (auto x = 0u; x < players.size(); x++) {
						std::cout << "Player " << x << ": " << player.name << "\n";
					};
				}
				else {
					std::cout << "Player |" << player.name << "| already in the system. \n";
				}
			}
			else {
				std::cout << "Can't extract player";
				std::cout << playerPacket << "\n";
			}
		}

		window.display();
	}

	return 0;
}