#include "Bsp.h"

Bsp::Bsp()
{
	root = nullptr;
}

void deleteTree(wallNode* temp)
{
	if (temp == nullptr)
		return;
	deleteTree(temp->frontChild);
	deleteTree(temp->backChild);
	delete temp;
}

Bsp::~Bsp()
{
	deleteTree(root);
	root = nullptr;
}

Bsp::Bsp(std::vector<geom::Wall> walls)
{
	build(walls);
}

void insertSingle(wallNode* top, wallNode* inserted)
{
	// printf("mag: %f\n", inserted->wall.getFace().getMagnitude());
	if (inserted->wall.getFace().getMagnitude() == 0)
	{
		delete inserted;
		return;
	}
	wallNode* temp = top;
	wallNode* prev = nullptr;
	bool inFront = false;

	while (temp != nullptr)
	{
		prev = temp;
		if (temp->wall.inFrontOf(inserted->wall))
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

	root = new wallNode(walls.back());
	walls.pop_back();

	while(walls.begin() != walls.end())
	{
		wallNode* inserted = new wallNode(walls.back());
		insertSingle(root, inserted);
		walls.pop_back();
	}
}

//pretty good start
void renderHelper(wallNode* node, sf::RenderTarget& window, Player&p)
{
	if (node == nullptr)
	{
		return;
	}
	//this is the bps right here
	if(!node->wall.inFrontOf(p.getLoc()))
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