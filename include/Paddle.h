#pragma once
#include <SFML/Graphics.hpp>

class Paddle {
public:
	sf::RectangleShape paddle;
	sf::Vector2f paddleSize;
	const float paddleSpeed = 400.f;
	float rightPaddleSpeed = 0.f;

	Paddle () {
		sf::Vector2f paddleSize (25, 100);
		// Create the left paddle	
		paddle.setSize (paddleSize - sf::Vector2f (3, 3));
		paddle.setOutlineThickness (3);
		paddle.setOutlineColor (sf::Color::Black);
		paddle.setFillColor (sf::Color (100, 100, 200));
		paddle.setOrigin (paddleSize / 2.f);
	}

	sf::Vector2f getPosition () {
		return paddle.getPosition ();
	}
};