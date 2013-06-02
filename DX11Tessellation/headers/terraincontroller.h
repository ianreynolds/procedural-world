#ifndef _TERRAINCONTROLLER_H_
#define _TERRAINCONTROLLER_H_

#include "controller.h"

class Terrain;

class TerrainController : public Controller
{
public:
	TerrainController(Terrain* terr);

	void Update(double deltaTime);
private:
	Terrain* terrain;
};

#endif