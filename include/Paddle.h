#pragma once
#include <SFML/Graphics.hpp>

class Paddle {
public:
	sf::RectangleShape paddle;
	sf::Vector2f paddleSize;
	const float paddleSpeed;
	float rightPaddleSpeed;

	Paddle () : paddleSize(25.0f, 100.0f), paddleSpeed(400.f), rightPaddleSpeed(0.f) {	
		// Create the left paddle	
		paddle.setSize (paddleSize - sf::Vector2f (3, 3));
		paddle.setOutlineThickness (1);
		paddle.setOutlineColor (sf::Color::White);
		paddle.setFillColor (sf::Color (172, 172, 172));
		paddle.setOrigin (paddleSize / 2.f);
	}

	sf::Vector2f getPosition () {
		return paddle.getPosition ();
	}
};