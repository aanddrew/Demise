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

	utils::Vector2d reach1(parent.getCenter(), inserted.getFace().getStart());
	utils::Vector2d reach2(parent.getCenter(), inserted.getFace().getEnd());

	float dot1 = inserted.getNormal().dot(reach1);
	float dot2 = inserted.getNormal().dot(reach2);

	if (abs(dot1) < 0.0001 || abs(dot2) < 0.0001)
		return false;

	return parent.inFrontOf(inserted.getFace().getStart())
				!=parent.inFrontOf(inserted.getFace().getEnd());
}

//returns array with two walls, the front and back wall
geom::Wall* split(geom::Wall& parent, geom::Wall& inserted)
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

std::vector<geom::Wall> doSplits(std::vector<geom::Wall> walls)
{
	//we have to compare each wall to every other wall
	for(int master = 0; master < walls.size(); master++)
	{
		for(int target = 0; target < walls.size(); target++)
		{
			//we don't need to compare against self
			if (master == target) continue;

			if (splits(walls.at(master), walls.at(target)))
			{
				//then its time to split the target
				geom::Wall* splits = split(walls.at(master), walls.at(target));

				walls.at(target) = splits[0];
				walls.push_back(splits[1]);
			}
		}
	}

	return walls;
}