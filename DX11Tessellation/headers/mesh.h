#ifndef _MESH_H_
#define _MESH_H_

#include <D3DX11.h>
#include "objectcomponent.h"

class Transform;

class Mesh : public ObjectComponent
{
public:
	Mesh();
	Mesh(ID3D11DeviceContext* context);
	virtual ~Mesh();

	virtual void Update(float deltaTime);
	virtual void Render() = 0;

	int GetIndexCount();
protected:
	ID3D11DeviceContext* deviceContext;

	unsigned int vertexCount;
	unsigned int indexCount;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};

inline int Mesh::GetIndexCount()
{
	return indexCount;
}

#endif