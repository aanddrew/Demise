#ifndef POINT2D_H
#define POINT2D_H

#include <math.h>

namespace utils
{
	//this class just stores 2d floating point coordinates
	//it has simple operations like move and getting the distance etc..
	class Point2d
	{
	private:
		float x;
		float y;
	public:
		Point2d();
		Point2d(float,float);
		float getX() const;
		float getY() const;
		float getDist(const Point2d&);
		void move(float, float);
		//unsure if we need these
		// void setX(float);
		// void setY(float);
	};
}
#endif