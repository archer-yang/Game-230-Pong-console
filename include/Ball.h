#pragma once
#include <SFML/Graphics.hpp>

class Ball {	
public:
	const float ballRadius = 10.f;
	float ballSpeed = 400.f;
	float ballAngle = 0.f; // to be changed later
	sf::CircleShape ball;

	Ball () {
		// Create the ball		
		ball.setRadius (ballRadius - 3);
		ball.setOutlineThickness (3);
		ball.setOutlineColor (sf::Color::Black);
		ball.setFillColor (sf::Color::White);
		ball.setOrigin (ballRadius / 2, ballRadius / 2);
	}

	sf::Vector2f getPosition () {
		return ball.getPosition ();
	}

	void setPosition (const sf::Vector2f& position) {
		ball.setPosition (position);
	}

	void setPosition (float offsetX, float offsetY) {
		ball.setPosition (offsetX, offsetY);
	}

	void move (float offsetX, float offsetY) {
		ball.move (offsetX, offsetY);
	}

	void move (const sf::Vector2f& offset) {
		ball.move (offset);
	}
};