#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>
#include "GameObject.h"
#include "d3d11renderer.h"

using namespace std;

class GameObject;

class Scene
{
public:
	Scene(D3D11Renderer* currentRenderer);
	~Scene();

	static Scene* GetCurrentScene();
	static void SetCurrentScene(Scene* scene);

	void AttachObject(GameObject* object);

	void StartScene();

	void UpdateScene(double deltaTime);
	void PostUpdateScene(double deltaTime);

	void UpdateRenderLists();
	void UpdateCameraLists();

	void RenderScene();
private:
	static Scene* currentScene;
	vector<GameObject*> objects;

	D3D11Renderer* renderer;
};

#endif
