#include "pch.h"
#include <iostream>
#include <fstream>
#include <istream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
sf::TcpSocket client;


bool contains(std::vector<std::string> v, std::string string1) {
	if (std::find(v.begin(), v.end(), string1) != v.end())
	{
		return true;
	}

	return false;
}


//void addPlayer(sf::TcpSocket client, playerClass player) {
//
//	players.insert(player);
//	sendPlayers();
//}

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
	return packet >> player.name >> player.xpos >> player.ypos >> player.xvel >> player.yvel;
}

//std::vector<playerClass> players;
//
//void sendPlayers() {
//	sf::Packet packet;
//
//	for (auto x = 0u; x < players.size(); x++) {
//		packet << players[x];
//	};
//
//	client.send(packet);
//}

int main() {
	bool running = true;
	while (running) {
		std::vector<std::string> players;
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

		sf::Packet receivedPacket;
		playerClass player;
		// TCP socket:
		if (client.receive(receivedPacket) != sf::Socket::Done)
		{
			// error...
		}
		else {
			receivedPacket >> player;
		}
		std::cout << receivedPacket << "\n";
	}
}