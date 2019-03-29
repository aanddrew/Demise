#include "Vector2d.h"

// #include <stdio.h>

namespace utils
{
	//default constructor has start and ends at (0,0), (0,0)
	Vector2d::Vector2d() : start(Point2d()), end(Point2d()),
												 dx(0.0), dy(0.0), fixedMagnitude(0.0){}
	//takes two points to be initialized
	Vector2d::Vector2d(Point2d start, Point2d end)
	: start(start),
	  end(end)
	{
		// printf("endX: %f, startX: %f\n", end.getX(), start.getX());
		updateDifferences();	
		updateMagnitude();
	}

	//private function, used to update dx and dy
	void Vector2d::updateDifferences()
	{
		dx = end.getX() - start.getX();
		dy = end.getY() - start.getY();
	}

	void Vector2d::updateMagnitude()
	{
		fixedMagnitude = getMagnitude();
	}

	void Vector2d::normalize()
	{
		float mag = getMagnitude();
		fixedMagnitude = mag;
		dx = dx/mag;
		dy = dy/mag;

		end = Point2d(start.getX() + dx, start.getY() + dy);
	}

	void Vector2d::move(float dx_, float dy_)
	{
		start.move(dx_, dy_);
		end.move(dx_, dy_);
	}

	void Vector2d::rotate(float dTheta)
	{
		float c = cos(dTheta);
		float s = sin(dTheta);
		dx = dx*c - dy*s;
		dy = dy*c + dx*s;

		end = Point2d(start.getX() + dx, start.getY() + dy);
	}

	void Vector2d::scale(float factor)
	{
		dx *= factor;
		dy *= factor;

		end = Point2d(start.getX() + dx, start.getY() + dy);
	}

	float Vector2d::getMagnitude()
	{
		return start.getDist(end);
	}

	float Vector2d::getFixedMagnitude() const
	{
		return fixedMagnitude;
	}

	//formula
	//a DOT b = a_x * b_x + a_y * b_y
	//we compare them as if they both begin at the origin
	float Vector2d::dot  (const Vector2d& target) const
	{
		return (this->getdx() * target.getdx()) 
					+(this->getdy() * target.getdy());
	}

	float Vector2d::cross(const Vector2d& target) const
	{
		return (this->getdx() * target.getdy()) 
					-(this->getdy() * target.getdx());
	}

	float Vector2d::getdx() const {return dx;}
	float Vector2d::getdy() const {return dy;}

	Point2d Vector2d::getStart() const {return start;}
	Point2d Vector2d::getEnd()   const {return end;  }
}