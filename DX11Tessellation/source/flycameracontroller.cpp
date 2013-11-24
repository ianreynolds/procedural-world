#include "FlyCameraController.h"

#include <Windows.h>
#include "transform.h"
#include "di8inputmanager.h"

#include "metrics.h"
#include "TerrainGenerator.h"

FlyCameraController::FlyCameraController(float cameraSpeed, float rotateSpeed)
	: speed(cameraSpeed), rotSpeed(rotateSpeed)
{
}

FlyCameraController::~FlyCameraController()
{
}

void FlyCameraController::Update(double deltaTime)
{
	float originDistance;
	XMStoreFloat(&originDistance, XMVector3Length(XMLoadFloat3(&transform->GetPosition())));

	Metrics::cameraAltitude = originDistance - 6371.0f;

	speed = max(Metrics::cameraAltitude / 10.0f, 0.1f);

	if(inputManager->IsKeyDown(DIK_LSHIFT) || inputManager->GetJoystickButton(7))
	{
		speed *= 10.0f;
	}

	if(inputManager->IsKeyDown(DIK_SPACE) || inputManager->GetJoystickButton(5))
	{
		transform->SetLookAt(XMFLOAT3(0.0f, 18000.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f));
	}

	XMFLOAT3 forwardAxis = transform->GetForwardVector();
	XMStoreFloat3(&forwardAxis, XMVector3Normalize(XMLoadFloat3(&forwardAxis)));

	XMFLOAT3 rightAxis = transform->GetRightVector();
	XMStoreFloat3(&rightAxis, XMVector3Normalize(XMLoadFloat3(&rightAxis)));

	if(inputManager->GetJoystickAxis(0, 1))
	{
		XMFLOAT3 translation;
		XMStoreFloat3(&translation, XMLoadFloat3(&forwardAxis) * static_cast<float>(deltaTime) * speed * -inputManager->GetJoystickAxis(0, 1));

		transform->Translate(translation);
	}
	else
	{
		if(inputManager->IsKeyDown(DIK_W))
		{
			XMFLOAT3 translation;
			XMStoreFloat3(&translation, XMLoadFloat3(&forwardAxis) * static_cast<float>(deltaTime) * speed);

			transform->Translate(translation);
		}

		if(inputManager->IsKeyDown(DIK_S))
		{
			XMFLOAT3 translation;
			XMStoreFloat3(&translation, -XMLoadFloat3(&forwardAxis) * static_cast<float>(deltaTime) * speed);

			transform->Translate(translation);
		}
	}

	
	if(inputManager->IsKeyDown(DIK_A) || inputManager->GetJoystickButton(4))
	{
		XMFLOAT3 translation;
		XMStoreFloat3(&translation, -XMLoadFloat3(&rightAxis) * static_cast<float>(deltaTime) * speed);

		transform->Translate(translation);
	}

	if(inputManager->IsKeyDown(DIK_D) || inputManager->GetJoystickButton(6))
	{
		XMFLOAT3 translation;
		XMStoreFloat3(&translation, XMLoadFloat3(&rightAxis) * static_cast<float>(deltaTime) * speed);

		transform->Translate(translation);
	}

	if(inputManager->GetJoystickAxis(0, 0))
	{
		transform->RotateLocalY(rotSpeed * static_cast<float>(deltaTime) * inputManager->GetJoystickAxis(0, 0));
	}
	else
	{
		if(inputManager->IsKeyDown(DIK_LEFT))
		{
			transform->RotateLocalY(-rotSpeed * static_cast<float>(deltaTime));
		}

		if(inputManager->IsKeyDown(DIK_RIGHT))
		{
			transform->RotateLocalY(rotSpeed * static_cast<float>(deltaTime));
		}
	}

	if(inputManager->GetJoystickAxis(1, 1))
	{
		transform->RotateLocalX(rotSpeed * static_cast<float>(deltaTime) * -inputManager->GetJoystickAxis(1, 1));
	}
	else
	{
		if(inputManager->IsKeyDown(DIK_UP))
		{
			transform->RotateLocalX(rotSpeed * static_cast<float>(deltaTime));
		}

		if(inputManager->IsKeyDown(DIK_DOWN))
		{
			transform->RotateLocalX(-rotSpeed * static_cast<float>(deltaTime));
		}
	}
	
	if(inputManager->GetJoystickAxis(1, 0))
	{
		transform->RotateLocalZ(rotSpeed * static_cast<float>(deltaTime) * -inputManager->GetJoystickAxis(1, 0));
	}
	else
	{
		if(inputManager->IsKeyDown(DIK_Q))
		{
			transform->RotateLocalZ(rotSpeed * static_cast<float>(deltaTime));
		}

		if(inputManager->IsKeyDown(DIK_E))
		{
			transform->RotateLocalZ(-rotSpeed * static_cast<float>(deltaTime));
		}
	}

	XMFLOAT3 spherePosition;

	XMStoreFloat3(&spherePosition, XMVector3Normalize(XMLoadFloat3(&transform->GetPosition())));

	XMFLOAT3 terrainPosition = TerrainGenerator::GetContinentPoint(spherePosition);

	float terrainHeight;
	XMStoreFloat(&terrainHeight, XMVector3Length(XMLoadFloat3(&terrainPosition)));

	float currentHeight;
	XMStoreFloat(&currentHeight, XMVector3Length(XMLoadFloat3(&transform->GetPosition())));

	if(currentHeight < terrainHeight + 0.01f)
	{
		XMFLOAT3 newPosition;
		XMStoreFloat3(&newPosition, XMLoadFloat3(&spherePosition) * (terrainHeight + 0.05f));

		transform->SetPosition(newPosition);
	}
	else if(currentHeight > 22000.0f)
	{
		XMFLOAT3 newPosition;
		XMStoreFloat3(&newPosition, XMLoadFloat3(&spherePosition) * 22000.0f);

		transform->SetPosition(newPosition);
	}
}