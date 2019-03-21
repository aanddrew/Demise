#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../geom/Wall.h"
#include "../utils/Point2d.h"
#include "../utils/Vector2d.h"

#include "scratch/Player.h"


//reads in a file of wall points ex:walls.txt
std::vector<geom::Wall> buildWalls(std::string fileName)
{
	std::vector<geom::Wall> walls;

	std::ifstream inFile(fileName);
	std::string temp;
	while(std::getline(inFile, temp))
	{
		//parse each line of input
		temp += ',';
		float pts[4];
		std::string built ="";
		int index = 0;
		for(int i = 0; i < temp.length(); i++)
		{
			if (temp[i] != ',')
			{
				built += temp[i];
			}
			else
			{
				pts[index] = std::stof(built);
				built = "";
				index++;
			}
		}
		//now its time to create the points of the wall
		walls.push_back(geom::Wall(
										utils::Point2d(pts[0], pts[1]),
										utils::Point2d(pts[2], pts[3])));
	}
	return walls;
}

//STRIKETHROUGH:failed attempt at new vector method for rendering
//might have to look back at the way I did it before
//and get this ball rolling...
//update: it's the next day. I am enlightened
/*
for the x coordinate on the screen, we can't just mutiply by a constant.
This is easy to see because if an object is any distance away from
	the viewer,
This results in the coordinate never ending up in the center...
FACEPALM
I have a solution to this problem

We give the camera (Player) a normal vector
The dot product of the toStart, toEnd vectors with that vector tells us this
		   wall.start()
	^	  /
x |	 /
	v	/__> 			p.getVec().
p   |
	  v
  p.getNormal();

 The distance x is the value we care about.
 x is toStart.dot(p.getNormal());

  The dod product of the normal with the wall.start tells us how far away in the 
  "effective x" direction (i.e. the distance perpendicularly from the player's 
  view vector)
*/
//this function is incredible
void drawWall(sf::RenderTarget& window, geom::Wall& wall, Player& p)
{
	//constants to use for 
	int k = 200;
	int C = 200;

	// printf("%f\n", p.getLoc().getY());
	utils::Point2d start = wall.getFace().getStart();
	utils::Point2d end   = wall.getFace().getEnd();
	utils::Vector2d toStart(p.getLoc(), start);
	utils::Vector2d toEnd(p.getLoc(), end);

	// printf("%f\n", toStart.getdy());
	// printf("%f\n", p.getVec().getdx() * toStart.getdy())

	//1 is the start, 2 is to the end
	float z1 = toStart.dot(p.getVec());
	float z2 = toEnd.dot(p.getVec());
	//if both parts of the wall are behind, dont draw it
	if (z1 < 0 && z2 < 0) return;

	float xDiff1 = toStart.dot(p.getNormal());
	float xDiff2 = toEnd.dot(p.getNormal());
	//these are extenders that come into play when the wall is behind the player
	float extendX1 = 0.0;
	float extendX2 = 0.0;
	if (z1 < 0)
	{
		xDiff1 = -1*xDiff1;
		//create a vector that points opposite the normal vector starting at the 
		//start of the wall...
		//we will use this to project this "anti-normal" onto 
		//the relative line z = 0 for the player's view.
		//we do this by scaling this unit vector by the z buffer from the player
		//divided by the relative z component of this vector with the player.
		utils::Vector2d temp(start, 
											 utils::Point2d(start.getX() - wall.getNormal().getdx(),
												  						start.getY() - wall.getNormal().getdy()));
		//make it so that this vector is a unit vector
		float zScaler = temp.dot(p.getVec());
		temp.scale(z1/zScaler);

		/*        
					similarX  
					________      ^
					|littleX/     |
		bigX  | V    /<scale|
		^-----|-----/-------p--->-----
		 \    |    /  
		  \  z|   /face   
  temp \  |  /    
		    \ | /     
		     \|/  
		     ^
			 temp makes a right angle with the line that z makes right here.
			 VERY IMPORTANT
			 -this drawing not to scale

		  Here is what the scene looks like currently
		  The player is facing forward and there is a wall whose starting point
		  	is behind the player
		  	and whose end point is in front of the player
		  The plan:
		  	Scale the face vector so that we only get the portion that is in
		  		front of the player
				Then we can just draw it like normal
			How to accomplish:
				SIMILAR TRIANGLES!!!! That's right I knew this stuff was useful
				lets set up some triangles here

				We want to figure out what the ratio of scale to the full 
					hypotenuse of the triangle is
				well, since these triangles are similar...
					scale = 1 - little_hypotenuse/big_hypotenuse
					little_hypotenuse/big_hypotenuse
				 =
				 	littleX/similarX

				 great.
				 Now we need to find littleX and similarX
				 similarX is easy.
				 	it's the component of face that is going in the same direction as
				 		p.getNormal() => face.dot(p.getNormal())
				 littleX, this requires some advanced similar triangles
				 	lets set something up
				 	 littleX    	     z
				 	 -------    =   -------
				 	   z 						  bigX
				 	 therefore,
				 	 littleX = z*z/bigX
				 	This is where that line comes from down there

				 Then we just calculate the scale,
				 scale the vector, and then we recalibrate our xDiff and z variables

			If you actually want to understand this, you need to get out pen and paper
			and probably build it yourself.
			It took me many days to figure this out but it works like a charm.
		*/

		//here comes some special math.... I can't believe I figured this out
		float bigX = temp.dot(p.getNormal());
		// printf("bigx :%f\n", bigX);
		float littleX = -1*z1*z1/bigX;

		utils::Vector2d face = wall.getFace();
		float similarX = -1*face.dot(p.getNormal());

		// printf("scale: %f\n", (0.98-(littleX/similarX)));

		//we give it 0.98 for some breathing room
		// face.scale(0.98- (littleX/similarX));
		//first we gotta reverse the vector because of the way I implemented scale
		utils::Vector2d tempFace(face.getEnd(), face.getStart());
		tempFace.scale(0.98-(littleX/similarX));
		//then we un-reverse it
		face = utils::Vector2d(tempFace.getEnd(), tempFace.getStart());

		toStart = utils::Vector2d(p.getLoc(), face.getStart());

		xDiff1 = toStart.dot(p.getNormal());
		z1 = toStart.dot(p.getVec());
	} 
	//we have to do some correcting if one of the points is behind the camera
	if (z2 < 0) 
	{
		xDiff2 = -1*xDiff2;

		utils::Vector2d temp(end, 
											 utils::Point2d(end.getX() - wall.getNormal().getdx(),
												  						end.getY() - wall.getNormal().getdy()));
		float zScaler = temp.dot(p.getVec());
		temp.scale(z2/zScaler);

		float bigX = temp.dot(p.getNormal());
		float littleX = -1*z2*z2/bigX;

		utils::Vector2d face = wall.getFace();
		float similarX = face.dot(p.getNormal());

		// printf("scale: %f\n", (1-littleX)/similarX);

		//we give it 0.98 for some breathing room
		face.scale(0.98- (littleX/similarX));
		toEnd = utils::Vector2d(p.getLoc(), face.getEnd());

		xDiff2 = toEnd.dot(p.getNormal());
		z2 = toEnd.dot(p.getVec());
	}
	// printf("%f\n", z1);
	int x1 = k*(xDiff1+extendX1)/z1;
	// int x1 = k*xDiff1/z1;
	int h1;
	if (extendX1 == 0)
		h1 = C/z1;
	else
		h1 = C*extendX1;

	// printf("%f\n", utils::Vector2d(p.getLoc(), wall.getFace().getEnd()).getdy());
	int x2 = k*(xDiff2+extendX2)/z2;
	// int x2 = k*xDiff2/z2;
	int h2;
	if (extendX2 == 0.0)
		h2 = C/z2;
	else
		h1 = C*extendX2;

	// printf("xDiffs: %f, %f\n", xDiff1, xDiff2);

	sf::ConvexShape shape;
	shape.setPointCount(4);
	if (z1 > 0)
	{
		shape.setPoint(0, sf::Vector2f(320+x1, 240+ h1/2));
		shape.setPoint(1, sf::Vector2f(320+x1, 240- h1/2));
	}
	else
	{
		shape.setPoint(1, sf::Vector2f(320+x1, 240+ h1/2));
		shape.setPoint(0, sf::Vector2f(320+x1, 240- h1/2));
	}

	if (z2 > 0)
	{
		shape.setPoint(2, sf::Vector2f(320+x2, 240- h2/2));
		shape.setPoint(3, sf::Vector2f(320+x2, 240+ h2/2));
	}
	else
	{
		shape.setPoint(3, sf::Vector2f(320+x2, 240- h2/2));
		shape.setPoint(2, sf::Vector2f(320+x2, 240+ h2/2));
	}

	// shape.setFillColor(sf::Color(rand()%255, rand()%255, rand()%255));
	shape.setFillColor(wall.getColor());

	window.draw(shape);
}

void drawCrosshair(sf::RenderTarget& window)
{
	int l = 16;
	int w = 2;
	sf::RectangleShape side(sf::Vector2f(l,w));
	sf::RectangleShape top (sf::Vector2f(w,l));

	side.setPosition(320-l/2, 240-w/2);
	 top.setPosition(320-w/2, 240-l/2);

	side.setFillColor(sf::Color::Red);
	 top.setFillColor(sf::Color::Red);

	window.draw(side);
	window.draw(top);
}

void handleInput(sf::Keyboard::Key key, bool isPressed, Player& p, sf::Time dt)
{
	p.handleKeyInput(key, isPressed);
	p.update(dt);
}

int main()
{
	printf("Application Starting...\n");
	std::vector<geom::Wall> walls = buildWalls("walls2.txt");
	// printf("%f\n", walls.at(0).getFace().getStart().getX());

	sf::RenderWindow window(sf::VideoMode(640,480), "3d test");
	sf::Mouse::setPosition(sf::Vector2i(320,240), window);
	window.setMouseCursorVisible(false);
	sf::Event event;

	sf::Clock clock;
	sf::Time dt;

	Player p;
	// p.rotate(M_PI);

	while(window.isOpen())
	{
		dt = clock.restart();

		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::KeyPressed:
				{
					handleInput(event.key.code, true, p, dt);
				}
					break;
				case sf::Event::KeyReleased:
				{
					handleInput(event.key.code, false, p, dt);
				}
					break;
			}
		}
		// p.move(0,-0.0001);
		// p.rotate(M_PI);
		p.rotate(-0.003* (sf::Mouse::getPosition(window).x - 320));
		sf::Mouse::setPosition(sf::Vector2i(320,240), window);

		// p.move(0.0001*p.getVec().getdx(), 0.0001*p.getVec().getdy());

		window.clear();

		p.update(dt);

		for(int i = 0; i < walls.size(); i++)
		{
			drawWall(window, walls.at(i), p);
		}
		drawCrosshair(window);
		window.display();	

		printf("fps: %f\n", 1.0/dt.asSeconds());
	}

	printf("Closing...\n");
	return 0;
}