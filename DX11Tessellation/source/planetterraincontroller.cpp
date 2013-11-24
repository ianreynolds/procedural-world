#include "PlanetTerrainController.h"
#include "planetterrain.h"

PlanetTerrainController::PlanetTerrainController(PlanetTerrain* terr)
	: terrain(terr)
{
}

void PlanetTerrainController::Update(double deltaTime)
{
	if(inputManager->IsKeyPressed(DIK_F))
	{
		terrain->EnableSphereCulling();
	}
	if(inputManager->IsKeyPressed(DIK_G))
	{
		terrain->DisableSphereCulling();
	}

	if(inputManager->IsKeyPressed(DIK_H))
	{
		terrain->EnableAABBCulling();
	}
	if(inputManager->IsKeyPressed(DIK_J))
	{
		terrain->DisableAABBCulling();
	}

	if(inputManager->IsKeyPressed(DIK_H))
	{
		terrain->EnableDistanceCulling();
	}
	if(inputManager->IsKeyPressed(DIK_J))
	{
		terrain->DisableDistanceCulling();
	}
}