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

//gonna look something like this
class Bsp
{
private:
	wallNode* root;
public:
	Bsp();
	Bsp(std::vector<geom::Wall>);
	void insert(wallNode*);
	void build(std::vector<geom::Wall>);
	void render(sf::RenderTarget& window, Player& p);
	void printTree();
};
#endif