#pragma once
#include <SFML/Graphics.hpp>

class PowerUp : public sf::Sprite {
public:
	sf::CircleShape ball;
	const float ballRadius;	

	PowerUp () : ballRadius (10.f){
		// Create the ball
		ball.setRadius (ballRadius);
		ball.setOrigin (ballRadius / 2, ballRadius / 2);
	}
};