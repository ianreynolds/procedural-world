#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include <map>
#include <vector>
#include "componentenums.h"
#include "objectcomponent.h"
#include "transform.h"
#include "controller.h"
#include "mesh.h"
#include "material.h"
#include "camera.h"
#include "terrain.h"

#include "patchmesh.h"
#include "planarcameracontroller.h"
#include "terraincontroller.h"
#include "rendercontroller.h"

using namespace std;

class GameObject
{
public:
	GameObject();
	// TODO: sort out a cloning method
	//GameObject(const GameObject &object);
	~GameObject();

	void Update(double deltaTime);
	void PostUpdate(double deltaTime);

	void AttachChildObject(GameObject* child);
	// TODO: implement this...
	void RemoveChildObject(GameObject* child) {};
	void AddComponent(ComponentEnums::ComponentType type, ObjectComponent* component);
	template <typename T> T* GetComponent();

	void SetParentObject(GameObject* parent);
	GameObject* GetParentObject();

	void Activate();
	void Deactivate();
	bool IsActive();

	GameObject* GetFirstChild();
	GameObject* GetNextChild();

	Transform* transform;
	Mesh* mesh;
	Controller* controller;
	Material* material;
	Camera* camera;
	Terrain* terrain;
private:
	map<ComponentEnums::ComponentType, ObjectComponent*> componentMap;
	vector<GameObject*> childObjects;
	unsigned int childIteratorIndex;

	GameObject* parentObject;

	bool active;
};

//TODO: use typeID and a GetType() bucket sort
template <typename T>
inline T* GameObject::GetComponent()
{
	T* component;

	map<ComponentEnums::ComponentType, ObjectComponent*>::iterator it;
	
	for(it = componentMap.begin(); it != componentMap.end(); ++it)
	{
		component = dynamic_cast<T*>((*it).second);
		if(component)
		{
			return component;
		}
	}

	return NULL;
}

#endif