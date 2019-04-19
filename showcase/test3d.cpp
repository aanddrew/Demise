#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../geom/Wall.h"
#include "../utils/Point2d.h"
#include "../utils/Vector2d.h"

// #include "../rendering/renderWall.h"
#include "Bsp.h"
// #include "BSP.h"

#include "scratch/Player.h"


//reads in a file of wall points ex:walls.txt
std::vector<geom::Wall> buildWalls(std::string fileName)
{
	std::vector<geom::Wall> walls;

	std::ifstream inFile(fileName);
	std::string temp;

	sf::Color currentColor;

	while(std::getline(inFile, temp))
	{
		if(temp.empty())
			continue;
		temp += ',';
		std::string built ="";
		if (temp[0] == 'C')
		{
			int colorVals[3];
			int colorI = 0;
			for(int i = 1; i < temp.length(); i++)
			{
				if (temp[i] != ',')
					built += temp[i];
				else
				{
					colorVals[colorI] = std::stoi(built);
					built = "";
					colorI++;
				}
			}
			currentColor = sf::Color(colorVals[0], colorVals[1], colorVals[2]);
			//parsing the color
			//set currentColor
			continue;
		}
		//parse each line of input
		float pts[4];
		int index = 0;
		bool comment = false;
		for(int i = 0; i < temp.length(); i++)
		{
			if (temp[i] == '#')
			{
				comment = true;
				break;
			}
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
		if (comment)
			continue;
		//now its time to create the points of the wall
		geom::Wall tempWall = geom::Wall(utils::Point2d(pts[0], pts[1]),
																		 utils::Point2d(pts[2], pts[3]));
		tempWall.setColor(currentColor);
		walls.push_back(tempWall);
	}
	return walls;
}

void drawCrosshair(sf::RenderTarget& window)
{
	int l = 16;
	int w = 2;
	sf::RectangleShape side(sf::Vector2f(l,w));
	sf::RectangleShape top (sf::Vector2f(w,l));

	side.setPosition(window.getView().getSize().x/2-l/2, window.getView().getSize().y/2-w/2);
	 top.setPosition(window.getView().getSize().x/2-w/2, window.getView().getSize().y/2-l/2);

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
	//set the seed for wall creation
	srand(1);
	printf("Application Starting...\n");
	std::vector<geom::Wall> walls = buildWalls("../maps/manualSplits.txt");
	// printf("%f\n", walls.at(0).getFace().getStart().getX());

	// Bsp bsp;
	// bsp.build(walls);
	// bsp.printTree();
	// std::vector<geom::Wall> walls;
	// walls.push_back(geom::Wall(utils::Point2d(-3,1), utils::Point2d(0,4)));
	// walls.push_back(geom::Wall(utils::Point2d(-6,1), utils::Point2d(-4,5)));
	// walls.push_back(geom::Wall(utils::Point2d(-8,1), utils::Point2d(-7,4)));
	Bsp bsp;
	bsp.build(walls);	

	//make a window that can't be resized
	sf::RenderWindow window(sf::VideoMode(1600,900), "3d test", 
												  sf::Style::Titlebar | sf::Style::Close);
	sf::Mouse::setPosition(sf::Vector2i(window.getView().getSize().x/2,window.getView().getSize().y/2), window);
	//hide that mouse cursor
	window.setMouseCursorVisible(false);

	//used to read keyboard input
	sf::Event event;

	//used to keep track of frameTime, fps for updats etc
	sf::Clock clock;
	sf::Time dt;

	//the player
	Player p;

	while(window.isOpen())
	{
		//grab the time of the last frame.
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
		//rotate the player's view based on how far they moved the mouse in the last frame
		p.rotate(-0.003* (sf::Mouse::getPosition(window).x - window.getView().getSize().x/2));
		//bring it back to th ecenter
		sf::Mouse::setPosition(sf::Vector2i(window.getView().getSize().x/2,window.getView().getSize().y/2), window);

		//rendering time
		window.clear();

		p.update(dt);

		bsp.traverse(window, p);

		drawCrosshair(window);

		window.display();	

		printf("fps: %f\n", 1.0/dt.asSeconds());
	}

	printf("Closing...\n");
	return 0;
}