#include "controller.h"

Controller::Controller()
{
	inputManager = DI8InputManager::GetInstance();
}

Controller::~Controller()
{
}

void Controller::Update(double deltaTime)
{
}

void Controller::PostUpdate(double deltaTime)
{
}