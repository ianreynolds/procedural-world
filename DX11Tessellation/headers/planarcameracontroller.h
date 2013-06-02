#ifndef _PLANARCAMERACONTROLLER_H_
#define _PLANARCAMERACONTROLLER_H_

#include "controller.h"

class PlanarCameraController : public Controller
{
public:
	PlanarCameraController(float cameraSpeed, float rotateSpeed);
	~PlanarCameraController();

	void Update(double deltaTime);
private:
	float speed;
	float rotSpeed;
};

#endif