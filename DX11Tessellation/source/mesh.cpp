#include "mesh.h"
#include "transform.h"

Mesh::Mesh()
{
}

Mesh::Mesh(ID3D11DeviceContext* context)
	: deviceContext(context)
{
}

Mesh::~Mesh()
{
}

void Mesh::Update(float deltaTime)
{
}