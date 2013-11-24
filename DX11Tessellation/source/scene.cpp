#include "Scene.h"

Scene* Scene::currentScene = NULL;

Scene::Scene(D3D11Renderer* currentRenderer)
	: renderer(currentRenderer)
{
}

Scene::~Scene()
{
	// deallocate all GameObjects
	vector<GameObject*>::iterator it;
	
	for(it = objects.begin(); it != objects.end(); ++it)
	{
		delete *it;
	}
}

Scene* Scene::GetCurrentScene()
{
	return currentScene;
}

void Scene::SetCurrentScene(Scene* scene)
{
	currentScene = scene;
}

void Scene::AttachObject(GameObject* object)
{
	objects.push_back(object);
}

void Scene::StartScene()
{
	UpdateRenderLists();
	UpdateCameraLists();
}

void Scene::UpdateScene(double deltaTime)
{
	// update all top-level objects
	for(unsigned int i = 0; i < objects.size(); ++i)
	{
		if(!objects[i]->GetParentObject() && objects[i]->IsActive())
		{
			objects[i]->Update(deltaTime);
		}
	}

	// TODO: can't use iterator as we resize the vector during some update functions
	//vector<GameObject*>::iterator it;

	//for(it = objects.begin(); it != objects.end(); ++it)
	//{
	//	if(!(*it)->GetParentObject() && (*it)->IsActive())
	//	{
	//		(*it)->Update(gameTime);
	//	}
	//}
}

void Scene::PostUpdateScene(double deltaTime)
{
	// post-update all top-level objects
	vector<GameObject*>::iterator it;
	
	for(it = objects.begin(); it != objects.end(); ++it)
	{
		if(!(*it)->GetParentObject() && (*it)->IsActive())
		{
			(*it)->PostUpdate(deltaTime);
		}
	}
}

void Scene::UpdateRenderLists()
{
	renderer->ClearRenderLists();
	
	// add all mesh components to render list
	vector<GameObject*>::iterator it;

	for(it = objects.begin(); it != objects.end(); ++it)
	{
		Mesh* meshComponent = (*it)->mesh;
		if(meshComponent)
		{
			renderer->AddRenderableMesh(meshComponent);
		}
		else
		{
			PlanetTerrain* terrainComponent = (*it)->terrain;
			if(terrainComponent)
			{
				renderer->AddRenderableTerrain(terrainComponent);
			}
		}
	}
}

void Scene::UpdateCameraLists()
{
	renderer->ClearCameraList();
	
	// add all camera components to camera list
	vector<GameObject*>::iterator it;

	for(it = objects.begin(); it != objects.end(); ++it)
	{
		Camera* cameraComponent = (*it)->camera;
		if(cameraComponent)
		{
			renderer->AddCamera(cameraComponent);
		}
	}
}

void Scene::RenderScene()
{
	// render all objects with a renderable component
	vector<GameObject*>::iterator it;
	
	for(it = objects.begin(); it != objects.end(); ++it)
	{
		if((*it)->IsActive())
		{
			Mesh* currentMesh = (*it)->mesh;
			Material* currentMaterial = (*it)->material;
			if(currentMesh && currentMesh->IsActive())
			{
				if(currentMaterial && currentMaterial->IsActive())
				{
					currentMaterial->StartEffect();
					currentMesh->Render();
					currentMaterial->EndEffect();
				}
				else
				{
					currentMesh->Render();
				}
			}
		}
	}
}