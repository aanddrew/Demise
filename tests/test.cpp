#include "utils/Point2d.h"
#include "utils/Vector2d.h"
#include "geom/Wall.h"

#include <stdio.h>

int main()
{
	utils::Point2d p(0.0, 0.0);
	utils::Point2d p2(0.3, 0.7);

	printf("distance: %f\n", p.getDist(p2));

	printf("%f\n", p.getX());
	printf("%f\n", p2.getX());

	utils::Point2d start;
	utils::Point2d e1(0.33333,0.1);
	utils::Point2d e2(0,3);

	utils::Vector2d v1(start, e1);
	utils::Vector2d v2(start, e2);

	printf("dot product is %f\n", v1.dot(v2));

	utils::Vector2d face(utils::Point2d(0,0), utils::Point2d(1,1));
	geom::Wall wall(face);

	printf("should be zero:\n");
	//obviously a vector DOT its normal is zero
	printf("face DOT normal: %f\n", wall.getFace().dot(wall.getNormal()));

	printf("Normal dx, dy: %f, %f\n", wall.getNormal().getdx(), 
																		wall.getNormal().getdy());

	return 0;
}