#ifndef BSP_H
#define BSP_H

#include "../geom/Wall.h"
#include "scratch/Player.h"
#include "../rendering/renderWall.h"

#include <vector>
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <stdio.h>

struct wallNode
{
	geom::Wall wall;
	wallNode* frontChild;
	wallNode* backChild;

	wallNode(geom::Wall wallIn)
	{
		wall = wallIn;
		frontChild = nullptr;
		backChild = nullptr;
	}
};

/*
Right now it only works with a pre-split map
*/
class Bsp
{
private:
	wallNode* root;
public:
	Bsp();
	~Bsp();
	Bsp(std::vector<geom::Wall>);
	void build(std::vector<geom::Wall>);
	void render(sf::RenderTarget& window, Player& p);
	void printTree();
};
#endif