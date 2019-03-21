#include "Point2d.h"

namespace utils
{
	//default constructor sets x and y to zero
	Point2d::Point2d() : x(0.0), y(0.0) {}
	//pass x and y as parameters
	Point2d::Point2d(float x_, float y_) : x(x_), y(y_) {}

	//moves the point dx, dy
	void Point2d::move(float dx, float dy) {x += dx; y+= dy;}
	//gets dist to another point
	float Point2d::getDist(const Point2d& target)	
	{
		float dx = (this->getX() - target.getX());
		float dy = (this->getY() - target.getY());
		return sqrt(dx*dx + dy*dy);
	}

	//getters
	float Point2d::getX() const {return x;}
	float Point2d::getY() const {return y;}
}