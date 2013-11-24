#include "gameobject.h"
#include "scene.h"

GameObject::GameObject()
	: parentObject(NULL), transform(NULL), mesh(NULL), controller(NULL), material(NULL), camera(NULL), terrain(NULL), active(true)
{
	// add transform to component list
	// NOTE: this must be done during construction to make the transform available for all other components
	transform = new Transform();

	// TODO: What is this? I really don't remember writing it...
	Transform* currentTransform((Transform*)componentMap[ComponentEnums::Transform]);

	AddComponent(ComponentEnums::Transform, transform);

	Scene::GetCurrentScene()->AttachObject(this);

	// set child iterator to start of vector
	childIteratorIndex = 0;
}

GameObject::~GameObject()
{
	// deallocate all components
	map<ComponentEnums::ComponentType, ObjectComponent*>::iterator it;
	
	for(it = componentMap.begin(); it != componentMap.end(); ++it)
	{
		delete (*it).second;
	}
}

void GameObject::Update(double deltaTime)
{
	//// update all components
	//map<ComponentEnums::ComponentType, ObjectComponent*>::iterator it;
	//
	//for(it = componentMap.begin(); it != componentMap.end(); ++it)
	//{
	//	if((*it).second->IsActive())
	//	{
	//		(*it).second->Update(deltaTime);
	//	}
	//}
	
	// update controller component
	if(controller)
	{
		controller->Update(deltaTime);
	}

	// update all child objects
	vector<GameObject*>::iterator objIt;

	for(objIt = childObjects.begin(); objIt != childObjects.end(); ++objIt)
	{
		if((*objIt)->IsActive())
		{
			(*objIt)->Update(deltaTime);
		}
	}
}

void GameObject::PostUpdate(double deltaTime)
{
	//// update all components
	//map<ComponentEnums::ComponentType, ObjectComponent*>::iterator it;
	//
	//for(it = componentMap.begin(); it != componentMap.end(); ++it)
	//{
	//	if((*it).second->IsActive())
	//	{
	//		(*it).second->PostUpdate(deltaTime);
	//	}
	//}

	// update controller component
	if(controller)
	{
		controller->Update(deltaTime);
	}

	// update all child objects
	vector<GameObject*>::iterator objIt;

	for(objIt = childObjects.begin(); objIt != childObjects.end(); ++objIt)
	{
		if((*objIt)->IsActive())
		{
			(*objIt)->PostUpdate(deltaTime);
		}
	}
}

void GameObject::AttachChildObject(GameObject* child)
{
	childObjects.push_back(child);
}

void GameObject::AddComponent(ComponentEnums::ComponentType type, ObjectComponent* component)
{
	// TODO: feedback if we attempt to replace the objects transform

	// TODO: add some sort of feedback here if we are replacing an existing object (existing object will still have memory allocated...)
	componentMap[type] = component;
	component->transform = transform;
	component->SetGameObject(this);

	switch(type)
	{
	case ComponentEnums::Mesh:
		mesh = dynamic_cast<Mesh*>(component);
		break;
	case ComponentEnums::Controller:
		controller = dynamic_cast<Controller*>(component);
		break;
	case ComponentEnums::Material:
		material = dynamic_cast<Material*>(component);
		break;
	case ComponentEnums::Camera:
		camera = dynamic_cast<Camera*>(component);
		break;
	case ComponentEnums::Terrain:
		terrain = dynamic_cast<PlanetTerrain*>(component);
		break;
	}
}

void GameObject::SetParentObject(GameObject* parent)
{
	parentObject = parent;
}

GameObject* GameObject::GetParentObject()
{
	return parentObject;
}

void GameObject::Activate()
{
	active = true;
}
	
void GameObject::Deactivate()
{
	active = false;
}

bool GameObject::IsActive()
{
	return active;
}

GameObject* GameObject::GetFirstChild()
{
	childIteratorIndex = 0;

	if(childIteratorIndex >= childObjects.size())
	{
		return NULL;
	}
	else
	{
		return childObjects[childIteratorIndex];
	}
}

GameObject* GameObject::GetNextChild()
{
	if(++childIteratorIndex >= childObjects.size())
	{
		childIteratorIndex = 0;
		return NULL;
	}
	else
	{
		return childObjects[childIteratorIndex];
	}
}