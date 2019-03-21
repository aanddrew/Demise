#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../geom/Wall.h"
#include "../utils/Point2d.h"
#include "../utils/Vector2d.h"

#include "scratch/Player.h"

int main()
{
	utils::Vector2d v1(utils::Point2d(0,0), utils::Point2d(0,1));
	utils::Vector2d v2(utils::Point2d(0,0), utils::Point2d(-2,3));
	utils::Vector2d v3(utils::Point2d(0,0), utils::Point2d(2,3));

	printf("dot 1: %f\n", v1.dot(v2));
	printf("dot 2: %f\n", v1.dot(v3));
	return 0;
}