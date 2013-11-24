#ifndef _PLANARCAMERACONTROLLER_H_
#define _PLANARCAMERACONTROLLER_H_

#include "controller.h"

class FlyCameraController : public Controller
{
public:
	FlyCameraController(float cameraSpeed, float rotateSpeed);
	~FlyCameraController();

	void Update(double deltaTime);
private:
	float speed;
	float rotSpeed;
};

#endif