#include "BSP.h"

BSP::BSP()
{
	root = nullptr;
}

//parent and inserted is just convention at this point, doesnt mean much in this
//context
//returns if the first node splits the second node in half
bool splits(geom::Wall& parent, geom::Wall& inserted)
{
	utils::Vector2d parFace = parent.getFace();
	utils::Vector2d insFace = inserted.getFace();

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

	return parent.inFrontOf(inserted.getFace().getStart())
				!=parent.inFrontOf(inserted.getFace().getEnd());
}

//returns array with two walls, the front and back wall
geom::Wall* split(geom::Wall parent, geom::Wall inserted)
{
	//to split the wall in two, we have to find the point of intersection
	//then we create two new walls.
	//we have to de-allocate the old wall node as well
	//to find point of intersection, we can use point-slope formula from algebra
	//the slopes of each are dy/dx, the points are the starting points
	// y = m1(x-x1) + y1 : (x1,y1) is face.start().getX()
	//set the equal and solve for x
	//then solve for y by plugging back in
	utils::Vector2d parFace = parent.getFace();
	utils::Vector2d insFace = inserted.getFace();

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
		insStart = inserted.getFace().getStart();
		b2 =-1*m2*insStart.getX() + insStart.getY();

		//since the parent wall is vertical,
		//it will split it at whatever x coordinate the wall exists at
		newX = parent.getFace().getStart().getX();
		//then we plug that x coordinate back into the wall's "equation"
		newY = m2*newX + b2;

		printf("m2: %f\n", m2);
		printf("newX %f, newY %f\n", newX, newY);
	}
	else if(insVertical)
	{
		printf("insVertical\n");

		//same thing for this one
		parStart = parent.getFace().getStart();
		b1 =-1*m1*parStart.getX() + parStart.getY();

		//same thing, we get the x, plug it into other lines equation
		newX = inserted.getFace().getStart().getX();

		//plug into equation...
		newY = m1*newX + b1;
	}
	else
	{
		parStart = parent.getFace().getStart();
		insStart = inserted.getFace().getStart();

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
	if (!parent.inFrontOf(frontWall))
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

void buildHelper(tree* node, std::vector<geom::Wall> walls, int depth)
{
	printf("depth: %d\n", depth);
	//if we reach a base, we're done
	if (node == nullptr)
		return;

	// printf("node %p\n", node);
	//give this node the list of walls,
	//THIS CAUSES A SEG FAULT??? I am too tired to debug this
	node->walls = walls;
	//then we decide the partition plane for this node
	//lets just shake up the walls randomly
	std::random_shuffle(walls.begin(), walls.end());
	//then we get the first wall in the vector and that will be the partition
	node->partition = walls.at(0);
	walls.erase(walls.begin());

	//then split the nodes into front and back
	std::vector<geom::Wall> frontWalls;
	std::vector<geom::Wall> backWalls;
	for(int i = 0; i < walls.size(); i++)
	{
		//then we got a splitter
		if (splits(node->partition, walls.at(i)))
		{
			geom::Wall* splits = split(node->partition, walls.at(i));
			// the split function can return dud walls, so we just have to check for
			//that before we add them to the list
			if (splits[0].getFace().getMagnitude() != 0)
				// printf("here\n" );
				frontWalls.push_back(splits[0]);
			if (splits[1].getFace().getMagnitude() != 0)
				backWalls.push_back(splits[1]);
			break;
		}

		if (walls.at(i).inFrontOf(node->partition))
		{
			frontWalls.push_back(walls.at(i));
		}
		else
		{
			backWalls.push_back(walls.at(i));
		}
	}
	//then create two sub trees and add those to the root

	if (frontWalls.size() != 0)
	{
		//then we create a new node
		tree* frontTree = new tree;
		node->front = frontTree;
		buildHelper(node->front, frontWalls, depth+1);
	}
	if (backWalls.size() != 0)
	{
		tree* backTree = new tree;
		node->back = backTree;
		buildHelper(node->back, backWalls, depth+1);
	}

	//important:
	//when are we done? idk
}

void BSP::build(std::vector<geom::Wall> walls)
{
	if (root == nullptr)
		root = new tree;
	buildHelper(root, walls, 0);
}

void renderHelper(tree* node, sf::RenderTarget& window, Player& p)
{
	if (node == nullptr)
		return;
	
	if (!node->partition.inFrontOf(p.getLoc()))
	{
		renderHelper(node->front, window, p);
		if (node->front == nullptr || node->back == nullptr)
		{
			// printf("size: %d\n", node->walls.size());
			renderWall(window, node->partition, p);
			for(int i = 0; i < node->walls.size(); i++)
			{
				renderWall(window, node->walls.at(i), p);
			}
			return;
		}
		renderHelper(node->back, window, p);
	}
	else
	{
		renderHelper(node->back, window, p);
		if (node->front == nullptr || node->back == nullptr)
		{
			// printf("size: %d\n", node->walls.size());
			renderWall(window, node->partition, p);
			for(int i = 0; i < node->walls.size(); i++)
			{
				renderWall(window, node->walls.at(i), p);
			}
			return;
		}
		renderHelper(node->front, window, p);
	}
}

void BSP::render(sf::RenderTarget& window, Player& p)
{
	// printf("rendering\n");
	renderHelper(root, window, p);
}