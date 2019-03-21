#include "Player.h"

Player::Player()
{
	//the player will be facing in the positive y direction at the start
	vec = utils::Vector2d(utils::Point2d(0,0), utils::Point2d(0,1));
	//the normal is facing 90degrees clockwise from the vec
	normal = utils::Vector2d(utils::Point2d(0,0), utils::Point2d(1,0));
}

void Player::handleKeyInput(sf::Keyboard::Key key, bool isPressed)
{
	//clever way to do it, instead of two separate keyUp and keyDowns
	if (key == sf::Keyboard::W)
		movingUp = isPressed;
	else if(key == sf::Keyboard::S)
		movingDown = isPressed;
	else if(key == sf::Keyboard::A)
		movingLeft = isPressed;
	else if(key == sf::Keyboard::D)
		movingRight = isPressed;
}

void Player::update(sf::Time dt)
{
	float xSpeedForward = vec.getdx() * speed * dt.asSeconds();
	float ySpeedForward = vec.getdy() * speed * dt.asSeconds();

	float xSpeedSide = normal.getdx() * speed * dt.asSeconds();
	float ySpeedSide = normal.getdy() * speed * dt.asSeconds();
	// printf("%f, %f\n", xSpeedSide, ySpeedSide);
	// printf("%f, %f\n", xSpeedForward, ySpeedForward);

	float xMoved = 0;
	float yMoved = 0;
	if (movingUp)
	{
		xMoved += xSpeedForward;
		yMoved += ySpeedForward;
	}
	if (movingDown)
	{
		xMoved -= xSpeedForward;
		yMoved -= ySpeedForward;
	}
	if(movingRight)
	{
		// printf("moving right\n");
		xMoved += xSpeedSide;
		yMoved += ySpeedSide;
	}
	if(movingLeft)
	{
		xMoved -= xSpeedSide;
		yMoved -= ySpeedSide;
	}

	//if the player is moving diagonally
	if ((movingLeft || movingRight ) && (movingUp || movingDown))
	{
		float OneOverROOT2 = 1.0/1.414213562;
		xMoved *= OneOverROOT2;
		yMoved *= OneOverROOT2;
	}

	move(xMoved, yMoved);
}

void Player::move(float dx, float dy)
{
	vec.move(dx,dy);
	normal.move(dx,dy);
}

// void Player::move(utils::Vector2d& change)
// {
// 	vec.move(change);
// }

void Player::rotate(float dTheta)
{
	vec.rotate(dTheta);
	vec.normalize();
	updateNormal();
	// printf("player size: %f\n", vec.getMagnitude());
}

void Player::updateNormal()
{
	//(h,k) => (k, -h);
	normal = utils::Vector2d(vec.getStart(), 
													utils::Point2d(vec.getStart().getX() + vec.getdy(),
													 								vec.getStart().getY() - vec.getdx()));
}

utils::Point2d Player::getLoc() {return vec.getStart();}
utils::Vector2d Player::getVec() {return vec;}
utils::Vector2d Player::getNormal() {return normal;}