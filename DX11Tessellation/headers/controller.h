#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "ObjectComponent.h"
#include "di8inputmanager.h"

class Transform;

class Controller : public ObjectComponent
{
public:
	Controller();
	virtual ~Controller() = 0;

	virtual void Update(double deltaTime);
	virtual void PostUpdate(double deltaTime);
protected:
	DI8InputManager* inputManager;
};

#endif