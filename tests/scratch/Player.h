#ifndef PLAYER_H
#define PLAYER_H

#include "../../utils/Vector2d.h"

#include <SFML/Graphics.hpp>

class Player
{
private:
	utils::Vector2d vec;
	utils::Vector2d normal;

	float speed = 3;

	//up and down really mean forward and backward
	//but since the game is effectively on a 2d plane, this still makes sense
	bool movingUp;
	bool movingDown;
	bool movingRight;
	bool movingLeft;
	void updateNormal();
public:
	Player();

	void handleKeyInput(sf::Keyboard::Key, bool);
	void update(sf::Time);

	void move(float, float);
	void rotate(float);
	void setRotation(const utils::Vector2d&);

	utils::Point2d getLoc();
	utils::Vector2d getVec();
	utils::Vector2d getNormal();
};

#endif