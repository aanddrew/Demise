#ifndef BSP_H
#define BSP_H

#include "../geom/Wall.h"
#include "scratch/Player.h"
#include "../rendering/renderWall.h"

#include <vector>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <stdio.h>

struct tree
{
	geom::Wall partition;
	std::vector<geom::Wall> walls;
	tree* front;
	tree* back;
};

class BSP
{
private:
	tree* root;
public:
	BSP();
	void build(std::vector<geom::Wall>);
	void render(sf::RenderTarget& window, Player& p);
	void printTree();
};

#endif