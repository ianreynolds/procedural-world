#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <D3D11.h>
#include <xnamath.h>
#include "objectcomponent.h"

// plane extraction from: http://www.cs.otago.ac.nz/postgrads/alexis/planeExtraction.pdf
// plane to origin distance from: http://mathworld.wolfram.com/HessianNormalForm.html
struct FrustumPlanes
{
	FrustumPlanes() {};
	FrustumPlanes(const XMFLOAT4X4 &matrix)
	{
		float length;
		// left plane
		normal[0] = XMFLOAT3(matrix._14 + matrix._11, matrix._24 + matrix._21, matrix._34 + matrix._31);
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&normal[0])));
		XMStoreFloat3(&normal[0], XMVector3Normalize(XMLoadFloat3(&normal[0])));
		originDistance[0] = (matrix._44 + matrix._41) / length;

		// right plane
		normal[1] = XMFLOAT3(matrix._14 - matrix._11, matrix._24 - matrix._21, matrix._34 - matrix._31);
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&normal[1])));
		XMStoreFloat3(&normal[1], XMVector3Normalize(XMLoadFloat3(&normal[1])));
		originDistance[1] = (matrix._44 - matrix._41) / length;

		// bottom plane
		normal[2] = XMFLOAT3(matrix._14 + matrix._12, matrix._24 + matrix._22, matrix._34 + matrix._32);
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&normal[2])));
		XMStoreFloat3(&normal[2], XMVector3Normalize(XMLoadFloat3(&normal[2])));
		originDistance[2] = (matrix._44 + matrix._42) / length;

		// top plane
		normal[3] = XMFLOAT3(matrix._14 - matrix._12, matrix._24 - matrix._22, matrix._34 - matrix._32);
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&normal[3])));
		XMStoreFloat3(&normal[3], XMVector3Normalize(XMLoadFloat3(&normal[3])));
		originDistance[3] = (matrix._44 - matrix._42) / length;

		// near plane
		normal[4] = XMFLOAT3(matrix._13, matrix._23, matrix._33);
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&normal[4])));
		XMStoreFloat3(&normal[4], XMVector3Normalize(XMLoadFloat3(&normal[4])));
		originDistance[4] = matrix._43 / length;

		// far plane
		normal[5] = XMFLOAT3(matrix._14 - matrix._13, matrix._24 - matrix._23, matrix._34 - matrix._33);
		XMStoreFloat(&length, XMVector3Length(XMLoadFloat3(&normal[5])));
		XMStoreFloat3(&normal[5], XMVector3Normalize(XMLoadFloat3(&normal[5])));
		originDistance[5] = (matrix._44 - matrix._43) / length;
	}
	XMFLOAT3 normal[6];
	float originDistance[6];
};

class Camera : public ObjectComponent
{
public:
	Camera(D3D11_VIEWPORT vp, float fovAngle, float aspectRatio, float nearZ, float farZ);
	~Camera();

	XMFLOAT4X4 Render();
	D3D11_VIEWPORT GetViewport();
	XMFLOAT4X4 GetProjectionMatrix();
	XMFLOAT4X4 GetViewProjectionMatrix();
	FrustumPlanes GetFrustum();
private:
	D3D11_VIEWPORT viewport;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 viewProjectionMatrix;

	FrustumPlanes frustum;
};

#endif