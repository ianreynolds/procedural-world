#include "camera.h"
#include "transform.h"

Camera::Camera(D3D11_VIEWPORT vp, float fovAngle, float aspectRatio, float nearZ, float farZ)
	: viewport(vp), frustum()
{
	// setup projection matrix
	XMStoreFloat4x4(&projectionMatrix, XMMatrixPerspectiveFovLH(fovAngle, aspectRatio, nearZ, farZ));

	// set view-projection matrix to vanilla projection initially
	viewProjectionMatrix = projectionMatrix;

	// set up initial view frustum
	frustum = FrustumPlanes(viewProjectionMatrix);
}

Camera::~Camera()
{
}

XMFLOAT4X4 Camera::Render()
{
	XMVECTOR position = XMLoadFloat3(&transform->GetPosition());
	XMVECTOR lookAt = position + XMLoadFloat3(&transform->GetForwardVector());
	XMVECTOR up = XMLoadFloat3(&transform->GetUpVector());

	XMMATRIX localViewMatrix = XMMatrixLookAtLH(position, lookAt, up);

	XMStoreFloat4x4(&viewMatrix, localViewMatrix);

	XMStoreFloat4x4(&viewProjectionMatrix, localViewMatrix * XMLoadFloat4x4(&projectionMatrix));

	frustum = FrustumPlanes(viewProjectionMatrix);

	return viewMatrix;
}

D3D11_VIEWPORT Camera::GetViewport()
{
	return viewport;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projectionMatrix;
}

XMFLOAT4X4 Camera::GetViewProjectionMatrix()
{
	return viewProjectionMatrix;
}

FrustumPlanes Camera::GetFrustum()
{
	return frustum;
}