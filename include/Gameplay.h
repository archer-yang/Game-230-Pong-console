#pragma once
#include <SFML/Graphics.hpp>
#include <ctime>
#include "../include/Ball.h"
#include "../include/Paddle.h"

class Gameplay {
private:
	const float pi = 3.14159f;
	const int gameWidth = 800;
	const int gameHeight = 600;
public:
	Ball pong;
	Paddle leftPaddle;
	Paddle rightPaddle;

	sf::Font font;
	sf::Text pauseMessage;
	sf::Clock AITimer;

	// Define the paddles properties
	const sf::Time AITime = sf::seconds (0.1f);	
	sf::Clock clock;
	bool isPlaying = false;
	sf::Event event;

	int init ();
};