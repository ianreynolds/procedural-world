#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include <D3D11.h>
#include <xnamath.h>

#include "objectcomponent.h"

// TODO: euler rotations will have absolutely no effect now
class Transform : public ObjectComponent
{
public:
	Transform(void);
	~Transform(void);

	void Update(float deltaTime);

	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3 &pos);
	void SetPosition(const XMFLOAT4 &pos);
	void SetRotation(float x, float y, float z);
	void SetRotation(const XMFLOAT3 &rot);
	void SetScale(float x, float y, float z);
	void SetScale(const XMFLOAT3 &scl);

	void SetLookAt(const XMFLOAT3 &pos, const XMFLOAT3 &target, const XMFLOAT3 &up);
	void SetLookTo(const XMFLOAT3 &pos, const XMFLOAT3 &direction, const XMFLOAT3 &up);

	void Translate(const XMFLOAT3 &trans);

	//void Rotate(const float x, const float y, const float z);
	//void Rotate(const XMFLOAT3 &rot);
	void RotateX(const float angle);
	void RotateY(const float angle);
	void RotateZ(const float angle);

	//void RotateLocal(const XMFLOAT3 &rot);
	void RotateLocalX(const float angle);
	void RotateLocalY(const float angle);
	void RotateLocalZ(const float angle);
	
	void RotateAxis(const XMFLOAT3 &axis, const float angle);
	//void RotateAbout(const XMFLOAT3 &point, const XMFLOAT3 &axis, const float angle);

	const XMFLOAT3 GetPosition() const;
	const XMFLOAT3 GetRotation() const;
	const XMFLOAT3 GetScale() const;
	const XMFLOAT4X4 GetWorldMatrix() const;
	const XMFLOAT4X4 GetWorldRotationMatrix() const;
	const XMFLOAT3 GetForwardVector() const;
	const XMFLOAT3 GetUpVector() const;
	const XMFLOAT3 GetRightVector() const;
protected:
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMFLOAT3 scale;

	XMFLOAT3 forwardVector;
	XMFLOAT3 upVector;
	XMFLOAT3 rightVector;

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 worldRotationMatrix;

	void UpdateWorldMatrix();
};

inline void Transform::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;

	UpdateWorldMatrix();
}

inline void Transform::SetPosition(const XMFLOAT3 &pos)
{
	position = pos;

	UpdateWorldMatrix();
}

inline void Transform::SetPosition(const XMFLOAT4 &pos)
{
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;

	UpdateWorldMatrix();
}

inline void Transform::SetRotation(float x, float y, float z)
{
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;

	UpdateWorldMatrix();
}

inline void Transform::SetRotation(const XMFLOAT3 &rot)
{
	rotation = rot;

	UpdateWorldMatrix();
}

inline void Transform::SetScale(float x, float y, float z)
{
	scale.x = x;
	scale.y = y;
	scale.z = z;

	UpdateWorldMatrix();
}

inline void Transform::SetScale(const XMFLOAT3 &scl)
{
	scale = scl;

	UpdateWorldMatrix();
}

inline void Transform::SetLookAt(const XMFLOAT3 &pos, const XMFLOAT3 &target, const XMFLOAT3 &up)
{
	position = pos;
	XMStoreFloat3(&forwardVector, XMVector3Normalize(XMLoadFloat3(&target) - XMLoadFloat3(&position)));
	XMStoreFloat3(&upVector, XMVector3Normalize(XMLoadFloat3(&up)));

	XMStoreFloat3(&rightVector, XMVector3Cross(XMLoadFloat3(&upVector), XMLoadFloat3(&forwardVector)));

	//XMMATRIX lookToMatrix = XMMatrixLookToLH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat3(&forwardVector), XMLoadFloat3(&upVector));
	//XMMATRIX lookAtMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat3(&forwardVector), XMLoadFloat3(&upVector));

	//XMFLOAT4 rotationQuat;

	//XMStoreFloat4(&rotationQuat, XMQuaternionRotationMatrix(lookToMatrix));

	//rotation.x = asin(2.0f * (rotationQuat.x * rotationQuat.y + rotationQuat.z * rotationQuat.w));
	//rotation.y = atan2(2.0f * (rotationQuat.y * rotationQuat.w - rotationQuat.x * rotationQuat.z), 1.0f - 2.0f * (rotationQuat.y * rotationQuat.y + rotationQuat.z * rotationQuat.z));
	//rotation.z = atan2(2.0f * (rotationQuat.x * rotationQuat.w - rotationQuat.y * rotationQuat.z), 1.0f - 2.0f * (rotationQuat.x * rotationQuat.x + rotationQuat.z * rotationQuat.z));
	
	UpdateWorldMatrix();
}

inline void Transform::SetLookTo(const XMFLOAT3 &pos, const XMFLOAT3 &direction, const XMFLOAT3 &up)
{
	position = pos;
	XMStoreFloat3(&forwardVector, XMVector3Normalize(XMLoadFloat3(&direction)));
	XMStoreFloat3(&upVector, XMVector3Normalize(XMLoadFloat3(&up)));

	XMStoreFloat3(&rightVector, XMVector3Cross(XMLoadFloat3(&upVector), XMLoadFloat3(&forwardVector)));

	//XMMATRIX lookToMatrix = XMMatrixLookToLH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat3(&forwardVector), XMLoadFloat3(&upVector));
	//XMMATRIX lookAtMatrix = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat3(&forwardVector), XMLoadFloat3(&upVector));

	//XMFLOAT4 rotationQuat;

	//XMStoreFloat4(&rotationQuat, XMQuaternionRotationMatrix(lookToMatrix));

	//rotation.x = asin(2.0f * (rotationQuat.x * rotationQuat.y + rotationQuat.z * rotationQuat.w));
	//rotation.y = atan2(2.0f * (rotationQuat.y * rotationQuat.w - rotationQuat.x * rotationQuat.z), 1.0f - 2.0f * (rotationQuat.y * rotationQuat.y + rotationQuat.z * rotationQuat.z));
	//rotation.z = atan2(2.0f * (rotationQuat.x * rotationQuat.w - rotationQuat.y * rotationQuat.z), 1.0f - 2.0f * (rotationQuat.x * rotationQuat.x + rotationQuat.z * rotationQuat.z));
	
	UpdateWorldMatrix();
}

inline void Transform::RotateX(const float angle)
{
	RotateAxis(XMFLOAT3(1.0f, 0.0f, 0.0f), angle);
	//XMVECTOR rotationQuat = XMQuaternionRotationNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), angle);

	//XMStoreFloat3(&forwardVector, XMVector3Rotate(XMLoadFloat3(&forwardVector), rotationQuat));
	//XMStoreFloat3(&upVector, XMVector3Rotate(XMLoadFloat3(&upVector), rotationQuat));
	//XMStoreFloat3(&rightVector, XMVector3Rotate(XMLoadFloat3(&rightVector), rotationQuat));

	//UpdateWorldMatrix();
}

inline void Transform::RotateY(const float angle)
{
	RotateAxis(XMFLOAT3(0.0f, 1.0f, 0.0f), angle);
}

inline void Transform::RotateZ(const float angle)
{
	RotateAxis(XMFLOAT3(0.0f, 0.0f, 1.0f), angle);
}

inline void Transform::RotateLocalX(const float angle)
{
	RotateAxis(rightVector, angle);
}

inline void Transform::RotateLocalY(const float angle)
{
	RotateAxis(upVector, angle);
}

inline void Transform::RotateLocalZ(const float angle)
{
	RotateAxis(forwardVector, angle);
}

inline void Transform::RotateAxis(const XMFLOAT3 &axis, const float angle)
{
	XMVECTOR rotAxis = XMVector3Normalize(XMLoadFloat3(&axis));

	XMVECTOR rotationQuat = XMQuaternionRotationNormal(rotAxis, angle);

	XMStoreFloat3(&forwardVector, XMVector3Rotate(XMLoadFloat3(&forwardVector), rotationQuat));
	XMStoreFloat3(&upVector, XMVector3Rotate(XMLoadFloat3(&upVector), rotationQuat));
	XMStoreFloat3(&rightVector, XMVector3Rotate(XMLoadFloat3(&rightVector), rotationQuat));

	UpdateWorldMatrix();
}

inline void Transform::Translate(const XMFLOAT3 &trans)
{
	position.x += trans.x;
	position.y += trans.y;
	position.z += trans.z;

	UpdateWorldMatrix();
}

void RotateX(const float angle);
	void RotateY(const float angle);

inline const XMFLOAT3 Transform::GetPosition() const
{
	return position;
}

inline const XMFLOAT3 Transform::GetRotation() const
{
	return rotation;
}

inline const XMFLOAT3 Transform::GetScale() const
{
	return scale;
}

inline const XMFLOAT4X4 Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

inline const XMFLOAT4X4 Transform::GetWorldRotationMatrix() const
{
	return worldRotationMatrix;
}

inline const XMFLOAT3 Transform::GetForwardVector() const
{
	return forwardVector;
}

inline const XMFLOAT3 Transform::GetUpVector() const
{
	return upVector;
}

inline const XMFLOAT3 Transform::GetRightVector() const
{
	return rightVector;
}

#endif