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
	sf::Event event;

	unsigned int p1Score = 0, p2Score = 0;
	enum states { INTRO, PLAYING, P1WON, P1LOST };
	int gameState = INTRO;

	int init ();
	void restart ();
};