#include "Wall.h"

namespace geom
{
	Wall::Wall() : Wall(utils::Point2d(0,0), utils::Point2d(0,0))
	{}
	Wall::Wall(utils::Vector2d faceIn)
	{
		face = faceIn;
		//by convention - our normal will face to the right of wherever
		// the the original vector is pointing
		center = utils::Point2d(faceIn.getStart().getX() + faceIn.getdx()/2,
														faceIn.getStart().getY() + faceIn.getdy()/2);
		//the normal points 90 degrees clockwise of wherever the original vector
		//was pointing
		//this is done by the mapping (x,y) => (y,-x);
		normal = utils::Vector2d(center, 
														 utils::Point2d(

													 	 				center.getX() 
													 	 			+ face.getdy(),

													 	 				center.getY() 
													 	 			- face.getdx()));
		normal.normalize();
		color = sf::Color(rand()%255, rand()%255, rand()%255);
	}

	Wall::Wall(utils::Point2d start, utils::Point2d end)
	: Wall(utils::Vector2d(start, end))
	{}

	//returns true if the /*parameter*/ is in front of *this*
	bool Wall::inFrontOf(const Wall& target) const
	{
		utils::Vector2d dist = utils::Vector2d( this->getCenter(), 
																					 target.getCenter());
		return this->getNormal().dot(dist) >= 0;
	}
	//same as above
	bool Wall::inFrontOf(const utils::Point2d& target) const
	{
		utils::Vector2d dist = utils::Vector2d( this->getCenter(), 
																					 target);
		return this->getNormal().dot(dist) >= 0;
	}

	utils::Point2d  Wall::getCenter()const {return center;}
	utils::Vector2d Wall::getFace()  const {return face;  }
	utils::Vector2d Wall::getNormal()const {return normal;}
	
	sf::Color 		  Wall::getColor() const {return color; }
}