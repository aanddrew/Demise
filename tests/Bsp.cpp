#include "Bsp.h"

Bsp::Bsp()
{
	root = nullptr;
}

Bsp::Bsp(std::vector<geom::Wall> walls)
{
	build(walls);
}

void Bsp::insert(wallNode* inserted)
{
	// printf("wall x: %f\n", node->wall.getFace().getStart().getX());
	if (root == nullptr)
	{
		root = inserted;
		return;
	}

	wallNode* temp = root;
	wallNode* prev = nullptr;
	bool inFront = false;

	while (temp != nullptr)
	{
		prev = temp;
		if (inserted->wall.inFrontOf(temp->wall))
		{
			temp = temp->frontChild;
			inFront = true;
		}
		else
		{
			temp = temp->backChild;
			inFront = false;
		}
	}
	//prev is now the parent of the Node to be inserted
	if (inFront)
	{
		prev->frontChild = inserted;
	}
	else
	{
		prev->backChild = inserted;
	}
}

void Bsp::build(std::vector<geom::Wall> walls)
{
	//scramble the list of walls,
	//this results in a more balanced tree usually, probably
	std::random_shuffle(walls.begin(), walls.end());

	while(walls.begin() != walls.end())
	{
		wallNode* inserted = new wallNode(walls.back());
		insert(inserted);
		walls.pop_back();
	}
}

// void renderNode(wallNode* node, sf::RenderTarget& window, Player& p)
// {

// }

//pretty good start
void renderHelper(wallNode* node, sf::RenderTarget& window, Player&p)
{
	if (node == nullptr)
	{
		return;
	}
	//this is the bps right here
	if(node->wall.inFrontOf(p.getLoc()))
	{
		//render all the walls 
		renderHelper(node->frontChild, window, p);
		renderWall(window, node->wall, p);
		renderHelper(node->backChild, window, p);
	}
	else
	{
		renderHelper(node->backChild, window, p);
		renderWall(window, node->wall, p);
		renderHelper(node->frontChild, window, p);
	}
}

void Bsp::render(sf::RenderTarget& window, Player& p)
{
	renderHelper(root, window, p);
}

void printTreeHelper(wallNode* node, int depth)
{
	if (node == nullptr)
		return;

	printf("Wall: %f, %f, %f, %f, depth: %d\n",
				 node->wall.getFace().getStart().getX(),
				 node->wall.getFace().getStart().getY(),
				 node->wall.getFace().getEnd().getX(),
				 node->wall.getFace().getEnd().getY(),
				 depth);
	// printf("		going back\n");
	printTreeHelper(node->backChild, depth +1);
	// printf("		going front\n");
	printTreeHelper(node->frontChild, depth +1);
}

void Bsp::printTree()
{
	if (root == nullptr)
	{
		printf("Tree is empty\n");
	}

	printTreeHelper(root, 0);
}