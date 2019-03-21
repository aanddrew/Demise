#include "utils/Point2d.h"
#include "utils/Vector2d.h"
#include "geom/Wall.h"

#include <stdio.h>

int main()
{
	utils::Point2d p1(-1,0);
	utils::Point2d p2( 0,3);
	geom::Wall w1(utils::Vector2d(p1, p2));

	utils::Point2d p3(3,-1);
	utils::Point2d p4(1,-2);
	geom::Wall w2(utils::Vector2d(p3, p4));

	printf("%s\n", (w1.inFrontOf(w2)) ? "front" : "back");

	printf("length of normal:%f\n", w1.getNormal().getMagnitude());
	printf("length of normal:%f\n", w2.getNormal().getMagnitude());

	return 0;
}