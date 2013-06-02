#include "transform.h"

Transform::Transform(void)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldRotationMatrix, XMMatrixIdentity());

	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	forwardVector = XMFLOAT3(0.0f, 0.0f, 1.0f);
	upVector = XMFLOAT3(0.0f, 1.0f, 0.0f);
	rightVector = XMFLOAT3(1.0f, 0.0f, 0.0f);
}

Transform::~Transform(void)
{
}

void Transform::Update(float deltaTime)
{
}

void Transform::UpdateWorldMatrix()
{
	XMMATRIX localWorldMatrix = XMMatrixIdentity();
	//localWorldRotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMMATRIX lookAtMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat3(&forwardVector), XMLoadFloat3(&upVector));
	XMMATRIX localWorldRotationMatrix = XMMatrixInverse(&XMVECTOR(), lookAtMatrix);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX translateMatrix = XMMatrixTranslation(position.x, position.y, position.z);

	localWorldMatrix *= scaleMatrix;
	localWorldMatrix *= localWorldRotationMatrix;
	localWorldMatrix *= translateMatrix;

	XMStoreFloat4x4(&worldMatrix, localWorldMatrix);
	XMStoreFloat4x4(&worldRotationMatrix, localWorldRotationMatrix);
}