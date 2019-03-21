#include "../utils/Point2d.h"
#include "../utils/Vector2d.h"
#include "../geom/Wall.h"

#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>

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

//draws a wall and its normal line
void drawWall(sf::RenderWindow& window, geom::Wall& wall)
{
	//draw wall
	utils::Vector2d face = wall.getFace();
	//we have to create a vertex array, and then draw it as that, okay
	sf::Vertex line[] =
	{
		sf::Vertex(sf::Vector2f(face.getStart().getX(), face.getStart().getY())),
		sf::Vertex(sf::Vector2f(face.  getEnd().getX(), face.  getEnd().getY()))
	};
	window.draw(line, 2, sf::Lines);

	//draw normal
	utils::Vector2d norm = wall.getNormal();
	sf::Vertex normal[] =
	{
		sf::Vertex(sf::Vector2f(norm.getStart().getX(), norm.getStart().getY())),
		sf::Vertex(sf::Vector2f(norm.  getEnd().getX(), norm.  getEnd().getY()))
	};
	window.draw(normal, 2, sf::Lines);
}
	
//draws a line based on a vector
void drawLine(sf::RenderWindow& window, utils::Vector2d& vec)
{
	// utils::Vector2d norm = wall.getNormal();
	sf::Vertex normal[] =
	{
		sf::Vertex(sf::Vector2f(vec.getStart().getX(), vec.getStart().getY())),
		sf::Vertex(sf::Vector2f(vec.  getEnd().getX(), vec.  getEnd().getY()))
	};
	window.draw(normal, 2, sf::Lines);
}

//get the mouse vector relative to (0,0)
utils::Vector2d getMouseVector(sf::RenderWindow& window)
{
	sf::Vector2i relCoords = sf::Mouse::getPosition(window);
	relCoords.x -= 320;
	relCoords.y -= 240;
	// printf("%d, %d\n", relCoords.x, relCoords.y);
	utils::Vector2d vec(utils::Point2d(), 
											utils::Point2d(relCoords.x, relCoords.y));
	vec.normalize();
	return vec;
}

//just moves the view arround, poorly
void handleInput(sf::Keyboard::Key key, bool isPressed, 
											 sf::View& camera)
{
	//clever way to do it, instead of two separate keyUp and keyDowns
	if (key == sf::Keyboard::W)
		camera.move(0, -0.1);
	else if(key == sf::Keyboard::S)
		camera.move(0, 0.1);
	else if(key == sf::Keyboard::A)
		camera.move(-0.1, 0);
	else if(key == sf::Keyboard::D)
		camera.move(0.1, 0);
}

int main()
{
	printf("Application starting...\n");
	//sfml stuff
	sf::RenderWindow window(sf::VideoMode(640,480), "Testing Walls.");
	sf::Event event;

	sf::Clock clock;
	sf::Time dt;

	//walls
	std::vector<geom::Wall> walls = buildWalls("walls2.txt");

	//window should be centered around 0,0
	sf::View camera = window.getView();
	camera.move(-320,-240);
	camera.zoom(0.05);
	window.setView(camera);

	while(window.isOpen())	
	{
		dt = clock.restart();
		//poll events
		while(window.pollEvent(event))
		{
			switch(event.type)
			{
				case sf::Event::KeyPressed:
					handleInput(event.key.code, true, camera);
					break;
				case sf::Event::KeyReleased:
					handleInput(event.key.code, false, camera);
					break;
				case sf::Event::Closed:
					window.close();
					break;
			}
		}	

		// printf("fps: %f\n", 1/dt.asSeconds());
		//render time
		window.clear();
		window.setView(camera);
		//render walls
		utils::Vector2d mv = getMouseVector(window);
		drawLine(window, mv);
		for(int i = 0; i < walls.size(); i++)
		{	
			if (walls.at(i).getNormal().dot(mv) < 0)
				drawWall(window, walls.at(i));
		}
		window.display();
	}

	printf("Closing...\n");

	return 0;
}