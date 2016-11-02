#pragma once
#include <SFML/Graphics.hpp>

class Ball {
public:
	sf::CircleShape ball;
	const float ballRadius;
	float ballSpeed;
	float ballAngle;
	float ballAcceleration;	
	

	Ball () : ballRadius(10.f), ballSpeed(400.0f), ballAngle(0.0f), ballAcceleration(25.0f) {
		// Create the ball
		ball.setRadius (ballRadius);
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