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
	float xpos = 100;
	float ypos = 100;
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	sf::Sprite image;
	sf::Texture texture;

	playerClass(sf::Sprite sprite, sf::Texture texture) {
		image = sprite;
		texture = texture;
	};

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
	return packet << player.name << player.xpos << player.ypos << player.xvel << player.yvel;;
}

sf::Packet& operator >>(sf::Packet& packet, playerClass& player)
{
	return packet >> player.name << player.xpos << player.ypos << player.xvel << player.yvel;
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

int main()
{
	sf::Socket::Status status = socket.connect("25.87.188.38", 53000);
	if (status != sf::Socket::Done)
	{
		// error...
	}

	sf::Texture spriteTexture;
	spriteTexture.loadFromFile("data/images/dragon_knight.png");
	sf::Sprite spriteObj(spriteTexture);

	std::vector<playerClass> players;

	std::string username;
	std::cout << "Choose username: ";
	std::cin >> username;
	std::cout << "Choosen username: " << username << "\n";

	playerClass player(spriteObj, spriteTexture);
	player.name = username;

	sendCharacter(player);

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), title, sf::Style::Titlebar | sf::Style::Close);

	sf::Packet packet;

	addPlayers(packet);

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

		for (auto x = 0u; x < players.size(); x++) {
			window.draw(players.at(x).image);
			players.at(x).update();
		};

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) player.up = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) player.down = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player.left = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player.right = true;

		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) player.up = false;
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) player.down = false;
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player.left = false;
		if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player.right = false;

		sendPlayers(players);

		window.display();

	}

	return 0;
}