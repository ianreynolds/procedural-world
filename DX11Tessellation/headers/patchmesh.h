#ifndef _PATCHMESH_H_
#define _PATCHMESH_H_

#include "mesh.h"

class PatchMesh : public Mesh
{
public:
	PatchMesh(ID3D11Device* device, ID3D11DeviceContext* context, float patchSize);
	~PatchMesh();

	void Render();
};

#endif