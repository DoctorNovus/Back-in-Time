#include "pch.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <set>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
sf::TcpSocket client;

struct playerClass {
public:
	std::string name;
	bool onGround = false;
	float xvel = 0;
	float yvel = 0;
	float xpos = 100;
	float ypos = 100;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	sf::Sprite image;
	sf::Texture texture;

	playerClass() {

	};

	void addSprite(sf::Sprite sprite) {
		image = sprite;
	}

	void addTexture(sf::Texture texture) {
		texture = texture;
	}

	void update() {
		if (up) {
			yvel = -0.5;
		};

		if (down) {
			yvel = 0.5;
		};

		if (left) {
			xvel = -0.5;
		};

		if (right) {
			xvel = 0.5;
		};

		if (!(up || down)) {
			yvel = 0;
		};

		if (!(left || right)) {
			xvel = 0;
		};

		xpos += xvel;
		ypos += yvel;

		image.setPosition(xpos, ypos);

		image.move(sf::Vector2f(xvel, yvel));
	}
};

sf::Packet& operator <<(sf::Packet& packet, const playerClass& player)
{
	return packet << player.name << player.xpos << player.ypos << player.xvel << player.yvel;
}

sf::Packet& operator >>(sf::Packet& packet, playerClass& player)
{
	return packet >> player.name << player.xpos << player.ypos << player.xvel << player.yvel;
}


std::vector<playerClass> players;


bool checkPlayer(playerClass player) {
	for (int x1 = 0; x1 < players.size(); ++x1) {
		if (players.at(x1).name == player.name) {
			return true;
		}
	}

	return false;
};

void updateAllPlayers() {
	if (players.size() > 0) {
		for (int x1 = 0; x1 < players.size(); ++x1) {
			sf::Packet packet;

			packet << players.at(x1);

			client.send(packet);
		}
	}
}

int main() {
	bool running = true;
	std::cout << "Server running..." << "\n";
	while (running) {
		sf::TcpListener listener;

		// bind the listener to a port
		if (listener.listen(53000) != sf::Socket::Done)
		{
			std::cout << "Unable to bind port |53000| to listener\n";
		}

		// accept a new connection
		if (listener.accept(client) != sf::Socket::Done)
		{
			std::cout << "Unable to accept client: \n";
		}
		else {
			std::cout << "Able to accept client: \n";
		}

		sf::Packet receivedPacket;
		// TCP socket:
		if (client.receive(receivedPacket) != sf::Socket::Done)
		{
			std::cout << "Error when recieving packets";
		}
		else {
			std::cout << "Recieved packet \n";

			std::string name;
			int xpos;
			int ypos;
			int xvel;
			int yvel;

			if (receivedPacket >> name >> xpos >> ypos >> xvel >> yvel) {

				std::cout << name << "\n";
				std::cout << "Received player: " << name << "\n";

				playerClass player;
				player.name = name;
				player.xpos = xpos;
				player.ypos = ypos;
				player.xvel = xvel;
				player.yvel = yvel;

				if (!checkPlayer(player)) {
					players.push_back(player);
					std::cout << player.name << " added to the system.";
					updateAllPlayers();
				}
				else {
					std::cout << player.name << " is already in the system.";
				}
			}
			else {
				std::cout << "Unable to extract player, the packet instead returns " << receivedPacket;
			}
		}
	}
}