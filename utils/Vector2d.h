#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "Point2d.h"
#include <math.h>

namespace utils
{
	class Vector2d
	{
	private:
		Point2d start;
		Point2d end;
		float dx;
		float dy;

		void updateDifferences();
		void updateMagnitude();
	public:
		Vector2d();
		Vector2d(Point2d, Point2d);
		float dot  (const Vector2d&) const;
		float cross(const Vector2d&) const;

		//pretty simple, but modifies the class
		void normalize();
		void move(float,float);
		void rotate(float);
		void scale(float);

		float getMagnitude();

		//getterssss
		float getdx() const;
		float getdy() const;

		Point2d getStart() const;
		Point2d getEnd() const;
	};
}

#endif