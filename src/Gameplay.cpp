#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include "../include/Gameplay.h"
#include <sstream>

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

	// Load the texture of the ball
	sf::Texture tex;
	if (!tex.loadFromFile ("resources/ball.png"))
		return EXIT_FAILURE;
	tex.setSmooth (true);
	pong.ball.setTexture (&tex);	

	// Initialize the pause message	
	pauseMessage.setFont (font);
	pauseMessage.setCharacterSize (40);
	pauseMessage.setPosition (170.f, 150.f);
	pauseMessage.setFillColor (sf::Color::White);
	pauseMessage.setString ("Welcome to SFML pong!\nPress space to start the game");

	sf::RectangleShape middleLine;
	middleLine.setFillColor (sf::Color::White);
	middleLine.setOutlineColor (sf::Color::White);
	middleLine.setOutlineThickness (1);
	middleLine.setPosition (gameWidth / 2, 0);
	middleLine.setSize (sf::Vector2f (0, gameHeight));

	sf::Text won ("You won!\nPress space to restart or\nesc to exit.", font, 40);
	won.setPosition (gameWidth / 2 - won.getGlobalBounds ().width / 2, gameHeight / 2 - won.getGlobalBounds ().height / 2);
	won.setFillColor (sf::Color::White);

	sf::Text lost ("You lost!\nPress space to restart or\nesc to exit.", font, 40);
	lost.setPosition (gameWidth / 2 - lost.getGlobalBounds ().width / 2, gameHeight / 2 - lost.getGlobalBounds ().height / 2);
	lost.setFillColor (sf::Color::White);

	sf::Text score ("0   0", font, 50);
	score.setPosition (gameWidth / 2 - score.getGlobalBounds ().width / 2, 40);
	score.setFillColor (sf::Color (239, 187, 56));	

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
				if (gameState != PLAYING) {
					p1Score = 0;
					p2Score = 0;

					std::stringstream str;
					str << p1Score << "   " << p2Score;
					score.setString (str.str ());
					score.setPosition (gameWidth / 2 - score.getGlobalBounds ().width / 2, 40);

					restart ();
				}
			}
		}

		if (gameState == PLAYING) {
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
				p2Score++;
				std::stringstream str;
				str << p1Score << "   " << p2Score;
				score.setString (str.str ());
				score.setPosition (gameWidth / 2 - score.getGlobalBounds ().width / 2, 40);

				restart ();
			}
			if (pong.ball.getPosition ().x + pong.ballRadius > gameWidth) {
				p1Score++;
				std::stringstream str;
				str << p1Score << "   " << p2Score;
				score.setString (str.str ());
				score.setPosition (gameWidth / 2 - score.getGlobalBounds ().width / 2, 40);

				restart ();
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
				//Accelerate the ball speed after each collision with the left paddle, increasing by 25 each time
				pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

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
				//Accelerate the ball speed after each collision with the right paddle, increasing by 25 each time
				pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

				if (pong.ball.getPosition ().y > rightPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (rightPaddle.paddle.getPosition ().x - pong.ballRadius - rightPaddle.paddleSize.x / 2 - 0.1f, pong.ball.getPosition ().y);
			}

			// Middle Left Paddle right edge collision
			if (pong.ball.getPosition ().x - pong.ballRadius < midLeftPaddle.paddle.getPosition ().x + midLeftPaddle.paddleSize.x / 2 &&
				pong.ball.getPosition ().x - pong.ballRadius > midLeftPaddle.paddle.getPosition ().x &&
				pong.ball.getPosition ().y + pong.ballRadius >= midLeftPaddle.paddle.getPosition ().y - midLeftPaddle.paddleSize.y / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius <= midLeftPaddle.paddle.getPosition ().y + midLeftPaddle.paddleSize.y / 2) {
				//Accelerate the ball speed after each collision with the left paddle, increasing by 25 each time
				pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

				if (pong.ball.getPosition ().y > midLeftPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (midLeftPaddle.paddle.getPosition ().x + pong.ballRadius + midLeftPaddle.paddleSize.x / 2 + 0.1f, pong.ball.getPosition ().y);
			}
			// Middle Left Paddle left edge collision
			if (pong.ball.getPosition ().x + pong.ballRadius > midLeftPaddle.paddle.getPosition ().x - midLeftPaddle.paddleSize.x / 2 &&
				pong.ball.getPosition ().x + pong.ballRadius < midLeftPaddle.paddle.getPosition ().x &&
				pong.ball.getPosition ().y + pong.ballRadius >= midLeftPaddle.paddle.getPosition ().y - midLeftPaddle.paddleSize.y / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius <= midLeftPaddle.paddle.getPosition ().y + midLeftPaddle.paddleSize.y / 2) {
				//Accelerate the ball speed after each collision with the right paddle, increasing by 25 each time
				pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

				if (pong.ball.getPosition ().y > midLeftPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (midLeftPaddle.paddle.getPosition ().x - pong.ballRadius - midLeftPaddle.paddleSize.x / 2 - 0.1f, pong.ball.getPosition ().y);
			}

			// Middle Right Paddle right edge collision
			if (pong.ball.getPosition ().x - pong.ballRadius < midRightPaddle.paddle.getPosition ().x + midRightPaddle.paddleSize.x / 2 &&
				pong.ball.getPosition ().x - pong.ballRadius > midRightPaddle.paddle.getPosition ().x &&
				pong.ball.getPosition ().y + pong.ballRadius >= midRightPaddle.paddle.getPosition ().y - midRightPaddle.paddleSize.y / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius <= midRightPaddle.paddle.getPosition ().y + midRightPaddle.paddleSize.y / 2) {
				//Accelerate the ball speed after each collision with the left paddle, increasing by 25 each time
				pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

				if (pong.ball.getPosition ().y > midRightPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (midRightPaddle.paddle.getPosition ().x + pong.ballRadius + midRightPaddle.paddleSize.x / 2 + 0.1f, pong.ball.getPosition ().y);
			}
			// Middle Right Paddle left edge collision
			if (pong.ball.getPosition ().x + pong.ballRadius > midRightPaddle.paddle.getPosition ().x - midRightPaddle.paddleSize.x / 2 &&
				pong.ball.getPosition ().x + pong.ballRadius < midRightPaddle.paddle.getPosition ().x &&
				pong.ball.getPosition ().y + pong.ballRadius >= midRightPaddle.paddle.getPosition ().y - midRightPaddle.paddleSize.y / 2 &&
				pong.ball.getPosition ().y - pong.ballRadius <= midRightPaddle.paddle.getPosition ().y + midRightPaddle.paddleSize.y / 2) {
				//Accelerate the ball speed after each collision with the right paddle, increasing by 25 each time
				pong.ballSpeed = pong.ballSpeed < 800 ? pong.ballSpeed + pong.ballAcceleration : 800;

				if (pong.ball.getPosition ().y > midRightPaddle.paddle.getPosition ().y)
					pong.ballAngle = pi - pong.ballAngle + (std::rand () % 20) * pi / 180;
				else
					pong.ballAngle = pi - pong.ballAngle - (std::rand () % 20) * pi / 180;

				ballSound.play ();
				pong.ball.setPosition (midRightPaddle.paddle.getPosition ().x - pong.ballRadius - midRightPaddle.paddleSize.x / 2 - 0.1f, pong.ball.getPosition ().y);
			}

			// detect if game is over
			if (p1Score >= 5 ) {
				gameState = P1WON;
			}				
			if (p2Score >= 5) {
				gameState = P1LOST;
			}				
		}

		// Clear the window
		window.clear (sf::Color (25, 147, 0));

		switch (gameState) {
		case INTRO:
			window.draw (pauseMessage);
			break;
		case PLAYING:
			window.draw (leftPaddle.paddle);
			window.draw (rightPaddle.paddle);
			window.draw (midLeftPaddle.paddle);
			window.draw (midRightPaddle.paddle);
			window.draw (pong.ball);
			window.draw (middleLine);
			window.draw (score);
			break;
		case P1WON:
			window.draw (won);
			break;
		case P1LOST:
			window.draw (lost);
			break;
		}

		// Display things on screen
		window.display ();
	}

	return EXIT_SUCCESS;
}

void Gameplay::restart () {
	gameState = PLAYING;
	clock.restart ();

	// Reset the position of the paddles and ball
	leftPaddle.paddle.setPosition (10 + leftPaddle.paddleSize.x / 2, gameHeight / 2);
	rightPaddle.paddle.setPosition (gameWidth - 10 - rightPaddle.paddleSize.x / 2, gameHeight / 2);
	midLeftPaddle.paddle.setPosition (gameWidth / 2 - midLeftPaddle.paddleSize.x * 2, gameHeight / 2);
	midRightPaddle.paddle.setPosition (gameWidth / 2 + midRightPaddle.paddleSize.x * 2, gameHeight / 2);

	pong.ball.setPosition (gameWidth / 2, gameHeight / 2);
	pong.ballSpeed = 400.f;

	// Reset the ball angle
	do {
		// Make sure the ball initial angle is not too much vertical
		pong.ballAngle = (std::rand () % 360) * 2 * pi / 360;
	} while (std::abs (std::cos (pong.ballAngle)) < 0.7f);
}
