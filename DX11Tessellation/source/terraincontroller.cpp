#include "terraincontroller.h"
#include "terrain.h"

TerrainController::TerrainController(Terrain* terr)
	: terrain(terr)
{
}

void TerrainController::Update(double deltaTime)
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