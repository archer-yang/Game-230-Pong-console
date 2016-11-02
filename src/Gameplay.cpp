#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "../include/Gameplay.h"
#include <iostream>

int Gameplay::init () {	
	std::srand (static_cast<unsigned int>(std::time (NULL)));

	// Create the window of the application
	sf::RenderWindow window (sf::VideoMode (gameWidth, gameHeight, 32), "Pong++");
	window.setFramerateLimit (60); // call it once, after creating the window

	// Load the sounds used in the game
	sf::SoundBuffer ballSoundBuffer;
	if (!ballSoundBuffer.loadFromFile ("resources/ball.wav"))
		return EXIT_FAILURE;
	sf::Sound ballSound (ballSoundBuffer);

	// Load the text font	
	if (!font.loadFromFile ("resources/sansation.ttf"))
		return EXIT_FAILURE;

	// Initialize the pause message	
	pauseMessage.setFont (font);
	pauseMessage.setCharacterSize (40);
	pauseMessage.setPosition (170.f, 150.f);
	pauseMessage.setFillColor (sf::Color::White);
	pauseMessage.setString ("Welcome to SFML pong!\nPress space to start the game");		

	while (window.isOpen ()) {
		// Handle events		
		while (window.pollEvent (event)) {
			// Window closed or escape key pressed: exit
			if ((event.type == sf::Event::Closed) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))) {
				window.close ();
				break;
			}

			// Space key pressed: play
			if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Space)) {
				if (!isPlaying) {
					// (re)start the game
					isPlaying = true;
					clock.restart ();

					// Reset the position of the paddles and ball
					leftPaddle.paddle.setPosition (10 + leftPaddle.paddleSize.x / 2, gameHeight / 2);
					rightPaddle.paddle.setPosition (gameWidth - 10 - rightPaddle.paddleSize.x / 2, gameHeight / 2);
					pong.ball.setPosition (gameWidth / 2, gameHeight / 2);

					// Reset the ball angle
					do {
						// Make sure the ball initial angle is not too much vertical
						pong.ballAngle = (std::rand () % 360) * 2 * pi / 360;
					} while (std::abs (std::cos (pong.ballAngle)) < 0.7f);
				}
			}
		}

		if (isPlaying) {
			float deltaTime = clock.restart ().asSeconds ();

			// Move the player's paddle
			if (sf::Keyboard::isKeyPressed (sf::Keyboard::Up) &&
				(leftPaddle.paddle.getPosition ().y - leftPaddle.paddleSize.y / 2 > 5.f)) {
				leftPaddle.paddle.move (0.f, -leftPaddle.paddleSpeed * deltaTime);
			}
			if (sf::Keyboard::isKeyPressed (sf::Keyboard::Down) &&
				(leftPaddle.paddle.getPosition ().y + leftPaddle.paddleSize.y / 2 < gameHeight - 5.f)) {
				leftPaddle.paddle.move (0.f, leftPaddle.paddleSpeed * deltaTime);
			}

			// Move the computer's paddle
			if (((rightPaddle.rightPaddleSpeed < 0.f) && (rightPaddle.paddle.getPosition ().y - rightPaddle.paddleSize.y / 2 > 5.f)) ||
				((rightPaddle.rightPaddleSpeed > 0.f) && (rightPaddle.paddle.getPosition ().y + rightPaddle.paddleSize.y / 2 < gameHeight - 5.f))) {
				rightPaddle.paddle.move (0.f, rightPaddle.rightPaddleSpeed * deltaTime);
			}

			// Update the computer's paddle direction according to the ball position
			if (AITimer.getElapsedTime () > AITime) {
				AITimer.restart ();
				if (pong.ball.getPosition ().y + pong.ballRadius > rightPaddle.paddle.getPosition ().y + rightPaddle.paddleSize.y / 2)
					rightPaddle.rightPaddleSpeed = rightPaddle.paddleSpeed;
				else if (pong.ball.getPosition ().y - pong.ballRadius < rightPaddle.paddle.getPosition ().y - rightPaddle.paddleSize.y / 2)
					rightPaddle.rightPaddleSpeed = -rightPaddle.paddleSpeed;
				else
					rightPaddle.rightPaddleSpeed = 0.f;
			}

			// Move the ball
			float factor = pong.ballSpeed * deltaTime;
			pong.ball.move (std::cos (pong.ballAngle) * factor, std::sin (pong.ballAngle) * factor);

			// Check collisions between the ball and the screen
			if (pong.ball.getPosition ().x - pong.ballRadius < 0.f) {
				isPlaying = false;
				pauseMessage.setString ("You lost!\nPress space to restart or\nescape to exit");
			}
			if (pong.ball.getPosition ().x + pong.ballRadius > gameWidth) {
				isPlaying = false;
				pauseMessage.setString ("You won!\nPress space to restart or\nescape to exit");
			}
			if (pong.ball.getPosition ().y - pong.ballRadius < 0.f) {
				ballSound.play ();
				pong.ballAngle = -pong.ballAngle;
				pong.ball.setPosition (pong.ball.getPosition ().x, pong.ballRadius + 0.1f);
			}
			if (pong.ball.getPosition ().y + pong.ballRadius > gameHeight) {
				ballSound.play ();
				pong.ballAngle = -pong.ballAngle;
				pong.ball.setPosition (pong.ball.getPosition ().x, gameHeight - pong.ballRadius - 0.1f);
			}

			// Check the collisions between the ball and the paddles
			// Left Paddle
			if (pong.ball.getPosition ().x - pong.ballRadius < leftPaddle.paddle.getPosition ().x + leftPaddle.paddleSize.x / 2 &&
				pong.ball.getPosition ().x - pong.ballRadius > leftPaddle.paddle.getPosition ().x &&
				pong.ball.getPosition ().y + pong.ballRadius >= leftPaddle.paddle.getPosition ().y - leftPaddle.paddleSize.y / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius <= leftPaddle.paddle.getPosition ().y + leftPaddle.paddleSize.y / 2) {

				pong.ballSpeed = (pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800);
				std::cout << pong.ballSpeed << std::endl;

				if (pong.ball.getPosition ().y > leftPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (leftPaddle.paddle.getPosition ().x + pong.ballRadius + leftPaddle.paddleSize.x / 2 + 0.1f, pong.ball.getPosition ().y);
			}

			// Right Paddle
			if (pong.ball.getPosition ().x + pong.ballRadius > rightPaddle.paddle.getPosition ().x - rightPaddle.paddleSize.x / 2 &&
				pong.ball.getPosition ().x + pong.ballRadius < rightPaddle.paddle.getPosition ().x &&
				pong.ball.getPosition ().y + pong.ballRadius >= rightPaddle.paddle.getPosition ().y - rightPaddle.paddleSize.y / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius <= rightPaddle.paddle.getPosition ().y + rightPaddle.paddleSize.y / 2) {

				pong.ballSpeed = (pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800);
				std::cout << pong.ballSpeed << std::endl;

				if (pong.ball.getPosition ().y > rightPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (rightPaddle.paddle.getPosition ().x - pong.ballRadius - rightPaddle.paddleSize.x / 2 - 0.1f, pong.ball.getPosition ().y);
			}
		}

		// Clear the window
		window.clear (sf::Color (50, 200, 50));

		if (isPlaying) {
			// Draw the paddles and the ball
			window.draw (leftPaddle.paddle);
			window.draw (rightPaddle.paddle);
			window.draw (pong.ball);
		} else {
			// Draw the pause message
			window.draw (pauseMessage);
		}

		// Display things on screen
		window.display ();
	}

	return EXIT_SUCCESS;
}
