#include "../geom/Wall.h"
#include <vector>

struct wallNode
{
	geom::Wall wall;
	wallNode* frontChild;
	wallNode* backChild;
};

//gonna look something like this
class BSP
{
private:
	wallNode* root;
public:
	BSP();
	BSP(std::vector<geom::Wall>);
	void build(std::vector<geom::Wall>);
	void render();
};