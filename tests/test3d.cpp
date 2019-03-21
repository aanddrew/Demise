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
	srand(0);
	printf("Application Starting...\n");
	std::vector<geom::Wall> walls = buildWalls("../maps/wallsbspTest.txt");
	// printf("%f\n", walls.at(0).getFace().getStart().getX());

	Bsp bsp;
	bsp.build(walls);
	bsp.printTree();

	sf::RenderWindow window(sf::VideoMode(1600,900), "3d test");
	sf::Mouse::setPosition(sf::Vector2i(window.getView().getSize().x/2,window.getView().getSize().y/2), window);
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
		p.rotate(-0.003* (sf::Mouse::getPosition(window).x - window.getView().getSize().x/2));
		sf::Mouse::setPosition(sf::Vector2i(window.getView().getSize().x/2,window.getView().getSize().y/2), window);
		
		window.clear();

		p.update(dt);

		bsp.render(window, p);

		drawCrosshair(window);

		window.display();	

		// printf("fps: %f\n", 1.0/dt.asSeconds());
	}

	printf("Closing...\n");
	return 0;
}