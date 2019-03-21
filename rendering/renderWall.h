#ifndef RENDERWALL_H
#define RENDERWALL_H

#include <SFML/Graphics.hpp>
#include "../geom/Wall.h"
#include "../tests/scratch/Player.h"

void renderWall(sf::RenderTarget& window, geom::Wall& wall, Player& p);

#endif