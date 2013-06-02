#ifndef _RENDERCONTROLLER_H_
#define _RENDERCONTROLLER_H_

#include "controller.h"

class D3D11Renderer;

class RenderController : public Controller
{
public:
	RenderController(D3D11Renderer* _renderer);

	void Update(double deltaTime);
private:
	D3D11Renderer* renderer;
};

#endif