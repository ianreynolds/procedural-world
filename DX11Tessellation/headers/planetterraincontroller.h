#ifndef _TERRAINCONTROLLER_H_
#define _TERRAINCONTROLLER_H_

#include "controller.h"

class PlanetTerrain;

class PlanetTerrainController : public Controller
{
public:
	PlanetTerrainController(PlanetTerrain* terr);

	void Update(double deltaTime);
private:
	PlanetTerrain* terrain;
};

#endif