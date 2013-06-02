#include "rendercontroller.h"
#include "d3d11renderer.h"

RenderController::RenderController(D3D11Renderer* _renderer)
	: renderer(_renderer)
{
}

void RenderController::Update(double deltaTime)
{
	if(inputManager->IsKeyPressed(DIK_Z))
	{
		renderer->SetSolidFill();
	}
	if(inputManager->IsKeyPressed(DIK_X))
	{
		renderer->SetWireframeFill();
	}

	if(inputManager->IsKeyPressed(DIK_F1))
	{
		renderer->SetFullscreen();
	}

	if(inputManager->IsKeyPressed(DIK_F2))
	{
		renderer->SetWindowed();
	}
}