#ifndef _OBJECTCOMPONENT_H_
#define _OBJECTCOMPONENT_H_

class GameObject;
class Transform;

class ObjectComponent
{
public:
	ObjectComponent();
	virtual ~ObjectComponent() = 0;

	// TODO: probably only need these in Controller components?
	virtual void Update(float deltaTime);
	virtual void PostUpdate(float deltaTime);
	
	void SetGameObject(GameObject* object);
	GameObject* GetGameObject() const;

	void Activate();
	void Deactivate();
	bool IsActive();

	Transform* transform;
protected:
	GameObject* gameObject;
	bool active;
};

inline GameObject* ObjectComponent::GetGameObject() const
{
	return gameObject;
}

#endif