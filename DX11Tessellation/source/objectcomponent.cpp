#include "objectcomponent.h"
#include "gameobject.h"

ObjectComponent::ObjectComponent()
	: gameObject(NULL), transform(NULL), active(true)
{
}

ObjectComponent::~ObjectComponent()
{
}

void ObjectComponent::SetGameObject(GameObject* object)
{
	gameObject = object;
	transform = object->transform;
}

void ObjectComponent::Update(float deltaTime)
{
}

void ObjectComponent::PostUpdate(float deltaTime)
{
}

void ObjectComponent::Activate()
{
	active = true;
}
	
void ObjectComponent::Deactivate()
{
	active = false;
}

bool ObjectComponent::IsActive()
{
	return active;
}
