#include "patchmesh.h"
#include "vertex.h"

PatchMesh::PatchMesh(ID3D11Device* device, ID3D11DeviceContext* context, float patchSize)
	: Mesh(context)
{
	HRESULT result;

	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set the number of vertices in the vertex array.
	vertexCount = 4;

	// Set the number of indices in the index array.
	indexCount = 4;

	// Create the vertex array.
	vertices = new Vertex[vertexCount];
	//if(!vertices)
	//{
	//	return false;
	//}

	// Create the index array.
	indices = new unsigned long[indexCount];
	//if(!indices)
	//{
	//	return false;
	//}

	// Load the vertex array with data.
	//vertices[0].position = XMFLOAT3(-1.0f, 0.0f, -1.0f);  // Bottom left.
	//vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	//vertices[1].position = XMFLOAT3(-1.0f, 0.0f, 1.0f);  // Top left.
	//vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	//vertices[2].position = XMFLOAT3(1.0f, 0.0f, 1.0f);  // Top right.
	//vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	//vertices[3].position = XMFLOAT3(1.0f, 0.0f, -1.0f);  // Bottom right.
	//vertices[3].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	float halfPatchSize = patchSize / 2.0f;

	vertices[0].position = XMFLOAT3(-halfPatchSize, 0.0f, -halfPatchSize);  // Bottom left.

	vertices[1].position = XMFLOAT3(-halfPatchSize, 0.0f, halfPatchSize);  // Top left.

	vertices[2].position = XMFLOAT3(halfPatchSize, 0.0f, halfPatchSize);  // Top right.

	vertices[3].position = XMFLOAT3(halfPatchSize, 0.0f, -halfPatchSize);  // Bottom right.

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Top right.
	indices[3] = 3;  // Bottom right.

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
	//if(FAILED(result))
	//{
	//	return false;
	//}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;
}

PatchMesh::~PatchMesh()
{
}

void PatchMesh::Render()
{
	// Set vertex buffer stride and offset.
	unsigned int stride = sizeof(Vertex); 
	unsigned int offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}