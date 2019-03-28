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
	printf("mag: %f\n", inserted->wall.getFace().getMagnitude());
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

//parent and inserted is just convention at this point, doesnt mean much in this
//context
//returns if the first node splits the second node in half
bool splits(wallNode* parent, wallNode* inserted)
{
	utils::Vector2d parFace = parent->wall.getFace();
	utils::Vector2d insFace = inserted->wall.getFace();

	if (parFace.getdx() == 0 && insFace.getdx() == 0)
	{
		return false;
	}

	float m1 = parFace.getdy()/parFace.getdx();
	float m2 = insFace.getdy()/insFace.getdx();

	// if (std::isnan(m1) && std::isnan(m2))
	// 	return false;

	if (abs(m1-m2) < 0.000001)
		return false;

	return parent->wall.inFrontOf(inserted->wall.getFace().getStart())
				!=parent->wall.inFrontOf(inserted->wall.getFace().getEnd());
}

//returns array with two walls, the front and back wall
geom::Wall* split(wallNode* parent, wallNode* inserted)
{
	//to split the wall in two, we have to find the point of intersection
	//then we create two new walls.
	//we have to de-allocate the old wall node as well
	//to find point of intersection, we can use point-slope formula from algebra
	//the slopes of each are dy/dx, the points are the starting points
	// y = m1(x-x1) + y1 : (x1,y1) is face.start().getX()
	//set the equal and solve for x
	//then solve for y by plugging back in
	utils::Vector2d parFace = parent->wall.getFace();
	utils::Vector2d insFace = inserted->wall.getFace();

	//okay so here's the deal, we have to deal with the case where the slope 
	//is infinite on one of the graphs

	//this is the flags for "vertical" walls that have slop = infinity
	bool parVertical = false;
	bool insVertical = false;

	//1 is for parent and 2 is for ins
	float m1;
	float m2;

	//slope m = dy/dx
	//parFace
	if (parFace.getdx() == 0)
		parVertical = true;
	else
		m1 = parFace.getdy()/parFace.getdx();
	//insFace
	if (insFace.getdx() == 0)
		insVertical = true;
	else
		m2 = insFace.getdy()/insFace.getdx();
	//done getting slopes

	float newX = 0.0;
	float newY = 0.0;

	//for if the wall is complicated
	//intercepts for parent, inserted respectively
	float b1, b2;
	utils::Point2d parStart, insStart;

	//I don't think its possible for both faces to be vertical
	//edit: I have "experimentally" confirmed this
	if (parVertical)
	{
		printf("parVertical\n");

		//we're really banking on the claime I made at the top of this if statement
		//being true
		insStart = inserted->wall.getFace().getStart();
		b2 =-1*m2*insStart.getX() + insStart.getY();

		//since the parent wall is vertical,
		//it will split it at whatever x coordinate the wall exists at
		newX = parent->wall.getFace().getStart().getX();
		//then we plug that x coordinate back into the wall's "equation"
		newY = m2*newX + b2;

		printf("m2: %f\n", m2);
		printf("newX %f, newY %f\n", newX, newY);
	}
	else if(insVertical)
	{
		printf("insVertical\n");

		//same thing for this one
		parStart = parent->wall.getFace().getStart();
		b1 =-1*m1*parStart.getX() + parStart.getY();

		//same thing, we get the x, plug it into other lines equation
		newX = inserted->wall.getFace().getStart().getX();

		//plug into equation...
		newY = m1*newX + b1;
	}
	else
	{
		parStart = parent->wall.getFace().getStart();
		insStart = inserted->wall.getFace().getStart();

		//b for y = mx + b, from y - y_0 = m(x-x_0)
		//								=>     y = m(x-x_0) + y_0
		//to solve for b = y intercept, = y in this eq
		//we substitute x = 0
		//								=> y = m(-x_0) + y_0
		//								=> b = m(-x_0) + y_0
		b1 =-1*m1*parStart.getX() + parStart.getY();
		b2 =-1*m2*insStart.getX() + insStart.getY();

		//we then set the two equatiosn equal to eachother
		//to solve for x
		//   m1x + b1 = m2x + b2
		//   (m1-m2)x = b2 - b1
		//		x = (b2-b1)/(m1-m2)

		// printf("%f, %f\n", m1, m2);
		newX = (b2-b1)/(m1-m2);
		//plug it back in to one of the equations
		newY = m1*newX + b1;
	}

	utils::Point2d newEnd(newX, newY);

	// printf("newEnd: %f, %f\n", newX, newY);

	geom::Wall frontWall(insFace.getStart(), newEnd);
	geom::Wall  backWall(newEnd, insFace.getEnd());

	//if they're not in the correct place, swap them
	if (!parent->wall.inFrontOf(frontWall))
	{
		geom::Wall temp = frontWall;
		frontWall = backWall;
		backWall = temp;
	}

	printf("getX, getY %f, %f\n", frontWall.getFace().getStart().getX(), frontWall.getFace().getStart().getY());

	geom::Wall* returned = new geom::Wall[2];
	returned[0] = frontWall;
	returned[1] = backWall;
	return returned;
}

void insertRecursive(wallNode* parent, wallNode* inserted)
{
	if(splits(parent, inserted))
	{
		//then we split the wall
		geom::Wall* splitWalls;
		splitWalls = split(parent, inserted);
		//no longer need inserted, we have two new walls
		delete inserted;

		geom::Wall frontWall = splitWalls[0];
		geom::Wall backWall = splitWalls[1];

		wallNode* frontNode = new wallNode(frontWall);
		wallNode*  backNode = new wallNode(backWall);

		//insert front wall
		if (parent->frontChild)
			insertRecursive(parent->frontChild, frontNode);
		else
			insertSingle(parent, frontNode);
		//insert back wall
		if (parent->backChild)
			insertRecursive(parent->backChild, backNode);
		else
			insertSingle(parent, backNode);
	}
	else
	{
		//insert wall normally
		insertSingle(parent, inserted);
		//if the inserted wall just split the parent in half, we have to repair it
		if (splits(inserted, parent))
		{
			geom::Wall* splitWalls;
			splitWalls = split(inserted, parent);
			//be very careful only to change the value of parent, beacuse its parent
			//still has a reference and we don't want to mess with that
			geom::Wall frontWall = splitWalls[0];
			geom::Wall backWall = splitWalls[1];

			parent->wall = frontWall;

			wallNode*  backNode = new wallNode(backWall);
			insertRecursive(parent, backNode);
		}
		return;
	}
}

void Bsp::insert(wallNode* inserted)
{
	// printf("wall x: %f\n", node->wall.getFace().getStart().getX());
	if (root == nullptr)
	{
		root = inserted;
		return;
	}

	// insertSingle(root, inserted);

	insertRecursive(root, inserted);
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