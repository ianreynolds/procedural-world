#include <list>
#include <stack>

#include "terrain.h"
#include "vertex.h"
#include "camera.h"
#include "transform.h"
#include "TerrainGenerator.h"
#include "d3d11renderer.h"

#include "metrics.h"

//////////////////QUADTREE//////////////////

#define PLANET_RADIUS 6371.0f
#define MAX_ELEVATION 14.0f
#define MAX_LOD 3
#define MAX_CPU_TESSELLATION_POWER 4.0f

#include <list>
#include <stack>

struct LODInfo
{
	XMFLOAT3 offset;
	float scale;
	float distance;

	LODInfo(XMFLOAT3 _offset, float _scale, float _distance)
		: offset(_offset), scale(_scale), distance(_distance)
	{
	}
};

float lodDistances[5] = { 2400.0f, 600.0f, 40.0f, 5.0f, 0.0f };
float lodIncrements[4] = { 450.0f, 140.0f, 8.75f, 1.0f };

TerrainQuadTreeNode* terrainQuadTreePosX;
TerrainQuadTreeNode* terrainQuadTreeNegX;
TerrainQuadTreeNode* terrainQuadTreePosY;
TerrainQuadTreeNode* terrainQuadTreeNegY;
TerrainQuadTreeNode* terrainQuadTreePosZ;
TerrainQuadTreeNode* terrainQuadTreeNegZ;

//////////////////QUADTREE//////////////////

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* context, D3D11Renderer* renderer, Shader* instancedGPUTerrainShader, Shader* lightingShader, RenderTexture* posBufferRenderTexture)
	: useSphereCulling(false), useAABBCulling(true), useDistanceCulling(true), device(device), deviceContext(context), d3d11Renderer(renderer),
	gpuInstancedShader(instancedGPUTerrainShader), lightingPassShader(lightingShader), positionBufferRenderTexture(posBufferRenderTexture),
	gpuVertexBuffer(nullptr), instanceBuffer(nullptr)
{
	// Initialise geometry buffers
	HRESULT result;

	unsigned int vertexCount;
	unsigned int indexCount;
	Vertex* vertices;
	VertexTex* verticesTex;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up terrain patch
	
	// Set the number of vertices in the vertex array.
	vertexCount = 12;

	// Set the number of indices in the index array.
	indexCount = 12;

	// Create the vertex array.
	vertices = new Vertex[vertexCount];

	// Create the index array.
	indices = new unsigned int[indexCount];

	float halfPatchSize = 0.0625f; // assumes 16 x 16 terrain patches on surface of unit cube

	vertices[0].position = XMFLOAT3(0.0f, -halfPatchSize, -halfPatchSize);  // Bottom left.
	vertices[3].position = XMFLOAT3(0.0f, -halfPatchSize, halfPatchSize);  // Top left.
	vertices[2].position = XMFLOAT3(0.0f, halfPatchSize, halfPatchSize);  // Top right.
	vertices[1].position = XMFLOAT3(0.0f, halfPatchSize, -halfPatchSize);  // Bottom right.

	vertices[4].position = XMFLOAT3(-halfPatchSize, 0.0f, -halfPatchSize);  // Bottom left.
	vertices[5].position = XMFLOAT3(-halfPatchSize, 0.0f, halfPatchSize);  // Top left.
	vertices[6].position = XMFLOAT3(halfPatchSize, 0.0f, halfPatchSize);  // Top right.
	vertices[7].position = XMFLOAT3(halfPatchSize, 0.0f, -halfPatchSize);  // Bottom right.

	vertices[8].position = XMFLOAT3(-halfPatchSize, -halfPatchSize, 0.0f);  // Bottom left.
	vertices[9].position = XMFLOAT3(-halfPatchSize, halfPatchSize, 0.0f);  // Top left.
	vertices[10].position = XMFLOAT3(halfPatchSize, halfPatchSize, 0.0f);  // Top right.
	vertices[11].position = XMFLOAT3(halfPatchSize, -halfPatchSize, 0.0f);  // Bottom right.

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Top right.
	indices[3] = 3;  // Bottom right.

	indices[4] = 4;  // Bottom left.
	indices[5] = 5;  // Top left.
	indices[6] = 6;  // Top right.
	indices[7] = 7;  // Bottom right.

	indices[8] = 8;  // Bottom left.
	indices[9] = 9;  // Top left.
	indices[10] = 10;  // Top right.
	indices[11] = 11;  // Bottom right.

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
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &gpuVertexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	// Set up screen sized quad
	
	// Set the number of vertices in the vertex array.
	vertexCount = 4;

	// Set the number of indices in the index array.
	indexCount = 4;

	// Create the vertex array.
	verticesTex = new VertexTex[vertexCount];

	// Create the index array.
	indices = new unsigned int[indexCount];

	verticesTex[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f);  // Top left.
	verticesTex[1].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	verticesTex[2].position = XMFLOAT3(1.0f, 1.0f, 0.0f);  // Top right.
	verticesTex[3].position = XMFLOAT3(1.0f, -1.0, 0.0f);  // Bottom right.

	verticesTex[0].texCoord = XMFLOAT2(0.0f, 0.0f);  // Top left.
	verticesTex[1].texCoord = XMFLOAT2(0.0f, 1.0f);  // Bottom left.
	verticesTex[2].texCoord = XMFLOAT2(1.0f, 0.0f);  // Top right.
	verticesTex[3].texCoord = XMFLOAT2(1.0f, 1.0);  // Bottom right.

	// Load the index array with data.
	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top left.
	indices[2] = 2;  // Top right.
	indices[3] = 3;  // Bottom right.

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexTex) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = verticesTex;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &screenQuadVertexBuffer);

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &screenQuadIndexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] verticesTex;
	vertices = 0;

	delete [] indices;
	indices = 0;

	// Set up the description of the instance buffer.
	D3D11_BUFFER_DESC instanceBufferDesc;

	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(PatchInstanceData) * 32768;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Create the instance buffer.
	device->CreateBuffer(&instanceBufferDesc, nullptr, &instanceBuffer);

	//////////////////QUADTREE//////////////////
	
	// TODO: delete trees at some point...
	terrainQuadTreePosY = new TerrainQuadTreeNode[342];

	int treeIterator = 0;
	int childIterator = 0;

	terrainQuadTreePosY[0].size = 2.0f;
	terrainQuadTreePosY[0].centre = XMFLOAT3(0.0f, 1.0f, 0.0f);

	while(treeIterator < 85)
	{
		float newSize = terrainQuadTreePosY[treeIterator].size / 2.0f;
		float halfSize = newSize / 2.0f;

		terrainQuadTreePosY[treeIterator].childNodes[0] = &terrainQuadTreePosY[++childIterator];
		terrainQuadTreePosY[treeIterator].childNodes[0]->size = newSize;
		terrainQuadTreePosY[treeIterator].childNodes[0]->centre = XMFLOAT3(	terrainQuadTreePosY[treeIterator].centre.x - halfSize,
																			terrainQuadTreePosY[treeIterator].centre.y,
																			terrainQuadTreePosY[treeIterator].centre.z - halfSize);

		terrainQuadTreePosY[treeIterator].childNodes[1] = &terrainQuadTreePosY[++childIterator];
		terrainQuadTreePosY[treeIterator].childNodes[1]->size = newSize;
		terrainQuadTreePosY[treeIterator].childNodes[1]->centre = XMFLOAT3(	terrainQuadTreePosY[treeIterator].centre.x + halfSize,
																			terrainQuadTreePosY[treeIterator].centre.y,
																			terrainQuadTreePosY[treeIterator].centre.z - halfSize);

		terrainQuadTreePosY[treeIterator].childNodes[2] = &terrainQuadTreePosY[++childIterator];
		terrainQuadTreePosY[treeIterator].childNodes[2]->size = newSize;
		terrainQuadTreePosY[treeIterator].childNodes[2]->centre = XMFLOAT3(	terrainQuadTreePosY[treeIterator].centre.x - halfSize,
																		terrainQuadTreePosY[treeIterator].centre.y,
																		terrainQuadTreePosY[treeIterator].centre.z + halfSize);

		terrainQuadTreePosY[treeIterator].childNodes[3] = &terrainQuadTreePosY[++childIterator];
		terrainQuadTreePosY[treeIterator].childNodes[3]->size = newSize;
		terrainQuadTreePosY[treeIterator].childNodes[3]->centre = XMFLOAT3(	terrainQuadTreePosY[treeIterator].centre.x + halfSize,
																			terrainQuadTreePosY[treeIterator].centre.y,
																			terrainQuadTreePosY[treeIterator].centre.z + halfSize);
		
		++treeIterator;
	}

	terrainQuadTreePosX = new TerrainQuadTreeNode[342];
	terrainQuadTreeNegX = new TerrainQuadTreeNode[342];
	terrainQuadTreeNegY = new TerrainQuadTreeNode[342];
	terrainQuadTreePosZ = new TerrainQuadTreeNode[342];
	terrainQuadTreeNegZ = new TerrainQuadTreeNode[342];

	XMFLOAT4X4 transformMatrix;

	XMStoreFloat4x4(&transformMatrix, XMMatrixRotationZ(XMConvertToRadians(-90.0f)));
	CopyQuadTree(terrainQuadTreePosX, terrainQuadTreePosY, transformMatrix);

	XMStoreFloat4x4(&transformMatrix, XMMatrixRotationZ(XMConvertToRadians(90.0f)));
	CopyQuadTree(terrainQuadTreeNegX, terrainQuadTreePosY, transformMatrix);

	XMStoreFloat4x4(&transformMatrix, XMMatrixRotationX(XMConvertToRadians(180.0f)));
	CopyQuadTree(terrainQuadTreeNegY, terrainQuadTreePosY, transformMatrix);

	XMStoreFloat4x4(&transformMatrix, XMMatrixRotationX(XMConvertToRadians(-90.0f)));
	CopyQuadTree(terrainQuadTreePosZ, terrainQuadTreePosY, transformMatrix);

	XMStoreFloat4x4(&transformMatrix, XMMatrixRotationX(XMConvertToRadians(90.0f)));
	CopyQuadTree(terrainQuadTreeNegZ, terrainQuadTreePosY, transformMatrix);

	//////////////////QUADTREE//////////////////
}

Terrain::~Terrain()
{
	if(gpuVertexBuffer)
	{
		gpuVertexBuffer->Release();
		gpuVertexBuffer = nullptr;
	}

	if(screenQuadVertexBuffer)
	{
		screenQuadVertexBuffer->Release();
		screenQuadVertexBuffer = nullptr;
	}

	if(screenQuadIndexBuffer)
	{
		screenQuadIndexBuffer->Release();
		screenQuadIndexBuffer = nullptr;
	}

	if(instanceBuffer)
	{
		instanceBuffer->Release();
		instanceBuffer = nullptr;
	}
}

void Terrain::CopyQuadTree(TerrainQuadTreeNode* destTree, TerrainQuadTreeNode* sourceTree, XMFLOAT4X4 transformMatrix)
{
	memcpy(destTree, sourceTree, sizeof(TerrainQuadTreeNode) * 342);

	for(int i = 0; i < 342; ++i)
	{
		XMStoreFloat3(&destTree[i].centre, XMVector3Transform(XMLoadFloat3(&destTree[i].centre), XMLoadFloat4x4(&transformMatrix)));
	}

	int treeIterator = 0;
	int childIterator = 0;

	while(treeIterator < 85)
	{
		destTree[treeIterator].childNodes[0] = &destTree[++childIterator];
		destTree[treeIterator].childNodes[1] = &destTree[++childIterator];
		destTree[treeIterator].childNodes[2] = &destTree[++childIterator];
		destTree[treeIterator].childNodes[3] = &destTree[++childIterator];

		++treeIterator;
	}
}

void Terrain::Render(Camera* camera, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	vector<PatchInstanceData> instanceVectorX;
	vector<PatchInstanceData> instanceVectorY;
	vector<PatchInstanceData> instanceVectorZ;
	
	FrustumPlanes frustum = camera->GetFrustum();
	XMVECTOR cameraPosition = XMLoadFloat3(&(camera->transform->GetPosition()));

	Metrics::treeTraversals = 0;
	Metrics::highestLOD = 0;
	Metrics::quadsInstances = 0;

	TraverseQuadTree(instanceVectorX, frustum, cameraPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, lodDistances[1], 1, Terrain::posX);
	TraverseQuadTree(instanceVectorX, frustum, cameraPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, lodDistances[1], 1, Terrain::negX);

	TraverseQuadTree(instanceVectorY, frustum, cameraPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, lodDistances[1], 1, Terrain::posY);
	TraverseQuadTree(instanceVectorY, frustum, cameraPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, lodDistances[1], 1, Terrain::negY);

	TraverseQuadTree(instanceVectorZ, frustum, cameraPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, lodDistances[1], 1, Terrain::posZ);
	TraverseQuadTree(instanceVectorZ, frustum, cameraPosition, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, lodDistances[1], 1, Terrain::negZ);

	//// clear the render texture
	//float clearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	//deviceContext->ClearRenderTargetView(positionBufferRenderTexture->GetRenderTargetView(), clearColor);

	//// set the position buffer texture 
	//d3d11Renderer->SetRenderTargetView(positionBufferRenderTexture->GetRenderTargetView());

	GPUInstDistContPerFrameParameter viewParameters(viewMatrix, projectionMatrix, camera->transform->GetPosition());
	gpuInstancedShader->SetPerFrameBuffers(deviceContext, &viewParameters);

	if(!instanceVectorX.empty())
	{
		//TODO: figure out why this function doesn't work
		//RenderInstanceVector(instanceVectorX, viewMatrix, projectionMatrix, camera->transform->GetPosition());
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;

		deviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

		memcpy(mappedSubresource.pData, &instanceVectorX[0], sizeof(PatchInstanceData) * instanceVectorX.size());

		deviceContext->Unmap(instanceBuffer, 0);

		RenderGPUPatchInstances();

		gpuInstancedShader->RenderInstanced(deviceContext, instanceVectorX.size(), 0, 0);
	}

	if(!instanceVectorY.empty())
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;

		deviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

		memcpy(mappedSubresource.pData, &instanceVectorY[0], sizeof(PatchInstanceData) * instanceVectorY.size());

		deviceContext->Unmap(instanceBuffer, 0);

		RenderGPUPatchInstances();

		gpuInstancedShader->RenderInstanced(deviceContext, instanceVectorY.size(), 4, 0);
	}

	if(!instanceVectorZ.empty())
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubresource;

		deviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

		memcpy(mappedSubresource.pData, &instanceVectorZ[0], sizeof(PatchInstanceData) * instanceVectorZ.size());

		deviceContext->Unmap(instanceBuffer, 0);

		RenderGPUPatchInstances();
		
		gpuInstancedShader->RenderInstanced(deviceContext, instanceVectorZ.size(), 8, 0);
	}

	bool wireframe = d3d11Renderer->IsWireframe();

	if(wireframe)
	{
		d3d11Renderer->SetWireframeFill();
	}

	//d3d11Renderer->SetRenderTargetView(nullptr);

	//LightingPassPerFrameParameter lightingParameters(positionBufferRenderTexture->GetShaderResourceView());
	//lightingPassShader->SetPerFrameBuffers(deviceContext, &lightingParameters);

	//RenderScreenQuad();
	//lightingPassShader->RenderInstanced(deviceContext, 4, 0, 0);

	//if(wireframe)
	//{
	//	d3d11Renderer->SetWireframeFill();
	//}
}

// NOTE: pass 0.0f into LODDistance for final LOD
void Terrain::TraverseQuadTree(vector<PatchInstanceData>& instanceVector, const FrustumPlanes& frustum, const XMVECTOR& cameraPosition, const XMVECTOR& offset, float scale, float lodDistance, int lod, CubeFace face)
{
	++Metrics::treeTraversals;
	Metrics::highestLOD = max(lod, Metrics::highestLOD);

	// TODO: could make these class members to save on making them every time
	list<TerrainQuadTreeNode*> nodeList;
	stack<LODInfo> lodStack;

	TerrainQuadTreeNode* currentNode = nullptr;
	switch(face)
	{
	case Terrain::posX:
		nodeList.push_back(&terrainQuadTreePosX[0]);
		break;
	case Terrain::negX:
		nodeList.push_back(&terrainQuadTreeNegX[0]);
		break;
	case Terrain::posY:
		nodeList.push_back(&terrainQuadTreePosY[0]);
		break;
	case Terrain::negY:
		nodeList.push_back(&terrainQuadTreeNegY[0]);
		break;
	case Terrain::posZ:
		nodeList.push_back(&terrainQuadTreePosZ[0]);
		break;
	case Terrain::negZ:
		nodeList.push_back(&terrainQuadTreeNegZ[0]);
		break;
	}

	// TODO: only need this once per frame
	XMFLOAT3 camPos;
	XMStoreFloat3(&camPos, cameraPosition);

	// TODO: only need this once per frame
	float maxViewDistance = sqrt((Metrics::cameraAltitude + PLANET_RADIUS) * (Metrics::cameraAltitude + PLANET_RADIUS) - PLANET_RADIUS * PLANET_RADIUS);

	// TODO: check the loop for common declarations and move them here...
	bool intersection = true;
	float dot;
	float distance;
	float halfWidth, halfHeight, halfDepth, radius;

	while(!nodeList.empty())
	{
		currentNode = nodeList.front();

		// http://www.flipcode.com/archives/Frustum_Culling.shtml
		
		XMVECTOR localCentre;

		switch(face)
		{
		case Terrain::posX:
			localCentre = XMLoadFloat3(&(currentNode->centre)) * XMVectorSet(1.0f, scale, scale, 0.0f) + offset;
			break;
		case Terrain::negX:
			localCentre = XMLoadFloat3(&(currentNode->centre)) * XMVectorSet(1.0f, scale, scale, 0.0f) + offset;
			break;
		case Terrain::posY:
			localCentre = XMLoadFloat3(&(currentNode->centre)) * XMVectorSet(scale, 1.0f, scale, 0.0f) + offset;
			break;
		case Terrain::negY:
			localCentre = XMLoadFloat3(&(currentNode->centre)) * XMVectorSet(scale, 1.0f, scale, 0.0f) + offset;
			break;
		case Terrain::posZ:
			localCentre = XMLoadFloat3(&(currentNode->centre)) * XMVectorSet(scale, scale, 1.0f, 0.0f) + offset;
			break;
		case Terrain::negZ:
			localCentre = XMLoadFloat3(&(currentNode->centre)) * XMVectorSet(scale, scale, 1.0f, 0.0f) + offset;
			break;
		}

		float halfSize = currentNode->size / 2.0f * scale;

		// AABB frustrum testing
			
		// calculate the extremities of the patch...
		// 0: centre point
		// 1-4: corner points
		// 5-8: outer points
		XMVECTOR lodPointVectors[9];
		lodPointVectors[0] = localCentre;

		if(face == Terrain::posX || face == Terrain::negX)
		{
			lodPointVectors[1] = localCentre + XMVectorSet(0.0f, halfSize, halfSize, 0.0f);
			lodPointVectors[2] = localCentre + XMVectorSet(0.0f, -halfSize, halfSize, 0.0f);
			lodPointVectors[3] = localCentre + XMVectorSet(0.0f, halfSize, -halfSize, 0.0f);
			lodPointVectors[4] = localCentre + XMVectorSet(0.0f, -halfSize, -halfSize, 0.0f);
			lodPointVectors[5] = localCentre + XMVectorSet(0.0f, 0.0f, -halfSize, 0.0f);
			lodPointVectors[6] = localCentre + XMVectorSet(0.0f, halfSize, 0.0f, 0.0f);
			lodPointVectors[7] = localCentre + XMVectorSet(0.0f, 0.0f, halfSize, 0.0f);
			lodPointVectors[8] = localCentre + XMVectorSet(0.0f, -halfSize, 0.0f, 0.0f);
		}
		else if(face == Terrain::posY || face == Terrain::negY)
		{
			lodPointVectors[1] = localCentre + XMVectorSet(halfSize, 0.0f, halfSize, 0.0f);
			lodPointVectors[2] = localCentre + XMVectorSet(-halfSize, 0.0f, halfSize, 0.0f);
			lodPointVectors[3] = localCentre + XMVectorSet(halfSize, 0.0f, -halfSize, 0.0f);
			lodPointVectors[4] = localCentre + XMVectorSet(-halfSize, 0.0f, -halfSize, 0.0f);
			lodPointVectors[5] = localCentre + XMVectorSet(0.0f, 0.0f, -halfSize, 0.0f);
			lodPointVectors[6] = localCentre + XMVectorSet(halfSize, 0.0f, 0.0f, 0.0f);
			lodPointVectors[7] = localCentre + XMVectorSet(0.0f, 0.0f, halfSize, 0.0f);
			lodPointVectors[8] = localCentre + XMVectorSet(-halfSize, 0.0f, 0.0f, 0.0f);
		}
		else if(face == Terrain::posZ || face == Terrain::negZ)
		{
			lodPointVectors[1] = localCentre + XMVectorSet(halfSize, halfSize, 0.0f, 0.0f);
			lodPointVectors[2] = localCentre + XMVectorSet(-halfSize, halfSize, 0.0f, 0.0f);
			lodPointVectors[3] = localCentre + XMVectorSet(halfSize, -halfSize, 0.0f, 0.0f);
			lodPointVectors[4] = localCentre + XMVectorSet(-halfSize, -halfSize, 0.0f, 0.0f);
			lodPointVectors[5] = localCentre + XMVectorSet(0.0f, -halfSize, 0.0f, 0.0f);
			lodPointVectors[6] = localCentre + XMVectorSet(halfSize, 0.0f, 0.0f, 0.0f);
			lodPointVectors[7] = localCentre + XMVectorSet(0.0f, halfSize, 0.0f, 0.0f);
			lodPointVectors[8] = localCentre + XMVectorSet(-halfSize, 0.0f, 0.0f, 0.0f);
		}

		XMFLOAT3 lodPoints[9];

		for(int j = 0; j < 9; ++j)
		{
			PointToSphere(lodPointVectors[j], PLANET_RADIUS);
			XMStoreFloat3(&lodPoints[j], lodPointVectors[j]);
		}

		XMFLOAT3 elevationDirection[2];
		XMStoreFloat3(&elevationDirection[0], XMVector3Normalize(lodPointVectors[0]) * MAX_ELEVATION);
		XMStoreFloat3(&elevationDirection[1], XMVector3Normalize(lodPointVectors[0]) * MAX_ELEVATION);

		elevationDirection[0].x = max(elevationDirection[0].x, 0.0f);
		elevationDirection[0].y = max(elevationDirection[0].y, 0.0f);
		elevationDirection[0].z = max(elevationDirection[0].z, 0.0f);

		elevationDirection[1].x = min(elevationDirection[1].x, 0.0f);
		elevationDirection[1].y = min(elevationDirection[1].y, 0.0f);
		elevationDirection[1].z = min(elevationDirection[1].z, 0.0f);

		float extremities[6];

		extremities[0] = extremities[1] = lodPoints[0].x;
		extremities[2] = extremities[3] = lodPoints[0].y;
		extremities[4] = extremities[5] = lodPoints[0].z;

		// TODO: could use min and max here...
		for(int j = 1; j < 9; j++)
		{
			if(lodPoints[j].x > extremities[0])
			{
				extremities[0] = lodPoints[j].x;
			}

			if(lodPoints[j].x < extremities[1])
			{
				extremities[1] = lodPoints[j].x;
			}

			if(lodPoints[j].y > extremities[2])
			{
				extremities[2] = lodPoints[j].y;
			}

			if(lodPoints[j].y < extremities[3])
			{
				extremities[3] = lodPoints[j].y;
			}

			if(lodPoints[j].z > extremities[4])
			{
				extremities[4] = lodPoints[j].z;
			}

			if(lodPoints[j].z < extremities[5])
			{
				extremities[5] = lodPoints[j].z;
			}
		}

		// TODO: must be a way to combine this and corner generation
		float cameraDistanceAABB = AABBDistance(extremities, camPos, elevationDirection);			
			
		if(!useDistanceCulling || cameraDistanceAABB <= maxViewDistance)
		{
			intersection = true;

			if(useSphereCulling)
			{
				// spherical frustrum testing
				halfWidth = abs(extremities[0] - extremities[1]) / 2.0f;
				halfHeight = abs(extremities[2] - extremities[3]) / 2.0f;
				halfDepth = abs(extremities[4] - extremities[5]) / 2.0f;
				radius = sqrt(halfWidth * halfWidth + halfHeight * halfHeight + halfDepth * halfDepth);

				XMVECTOR aabbCentre = XMVectorSet(extremities[1] + halfWidth, extremities[3] + halfHeight, extremities[5] + halfDepth, 0.0f);
			
				for(int j = 0; j < 6; j++)
				{
					XMStoreFloat(&dot, XMVector3Dot(XMLoadFloat3(&frustum.normal[j]), aabbCentre));
					distance = dot + frustum.originDistance[j];

					if(distance < -radius)
					{
						intersection = false;
						break;
					}
				}
			}
		
			if(intersection)
			{
				if(useAABBCulling)
				{
					XMVECTOR corners[8];

					corners[0] = XMVectorSet(extremities[0] + elevationDirection[0].x, extremities[2] + elevationDirection[0].y, extremities[4] + elevationDirection[0].z, 0.0f);
					corners[1] = XMVectorSet(extremities[0] + elevationDirection[0].x, extremities[2] + elevationDirection[0].y, extremities[5] + elevationDirection[1].z, 0.0f);
					corners[2] = XMVectorSet(extremities[0] + elevationDirection[0].x, extremities[3] + elevationDirection[1].y, extremities[4] + elevationDirection[0].z, 0.0f);
					corners[3] = XMVectorSet(extremities[0] + elevationDirection[0].x, extremities[3] + elevationDirection[1].y, extremities[5] + elevationDirection[1].z, 0.0f);
					corners[4] = XMVectorSet(extremities[1] + elevationDirection[1].x, extremities[2] + elevationDirection[0].y, extremities[4] + elevationDirection[0].z, 0.0f);
					corners[5] = XMVectorSet(extremities[1] + elevationDirection[1].x, extremities[2] + elevationDirection[0].y, extremities[5] + elevationDirection[1].z, 0.0f);
					corners[6] = XMVectorSet(extremities[1] + elevationDirection[1].x, extremities[3] + elevationDirection[1].y, extremities[4] + elevationDirection[0].z, 0.0f);
					corners[7] = XMVectorSet(extremities[1] + elevationDirection[1].x, extremities[3] + elevationDirection[1].y, extremities[5] + elevationDirection[1].z, 0.0f);

					for(int j = 0; j < 6; ++j)
					{
						int inCount = 8;

						for(int k = 0; k < 8; ++k)
						{
							XMStoreFloat(&dot, XMVector3Dot(XMLoadFloat3(&frustum.normal[j]), corners[k]));
							distance = dot + frustum.originDistance[j];

							if(distance < 0.0f)
							{
								inCount--;
							}
						}

						if(0 == inCount)
						{
							intersection = false;
							break;
						}
					}
				}

				if(intersection)
				{
					if(currentNode->childNodes[0])
					{
						for(int j = 0; j < 4; ++j)
						{
							nodeList.push_back(currentNode->childNodes[j]);
						}
					}
					else
					{
						float cameraDistance = AABBDistance(extremities, camPos, nullptr);
					
						if((cameraDistance < lodDistance) && (lod < MAX_LOD))
						{
							XMFLOAT3 newOffset;
							XMStoreFloat3(&newOffset, localCentre);

							if(face == Terrain::posX || face == Terrain::negX)
								newOffset.x = 0.0f;
							else if(face == Terrain::posY || face == Terrain::negY)
								newOffset.y = 0.0f;
							else if(face == Terrain::posZ || face == Terrain::negZ)
								newOffset.z = 0.0f;

							lodStack.push(LODInfo(newOffset, scale / 16.0f, lodDistances[lod + 1]));
						}
						else
						{
							XMFLOAT3 instancePosition;
							XMStoreFloat3(&instancePosition, localCentre);

							XMFLOAT3 instanceScale;

							++Metrics::quadsInstances;

							switch(face)
							{
							case Terrain::posX:
								instanceScale = XMFLOAT3(1.0f, scale, scale);
								break;
							case Terrain::negX:
								instanceScale = XMFLOAT3(1.0f, -scale, scale);
								break;
							case Terrain::posY:
								instanceScale = XMFLOAT3(scale, 1.0f, scale);
								break;
							case Terrain::negY:
								instanceScale = XMFLOAT3(-scale, 1.0f, scale);
								break;
							case Terrain::posZ:
								instanceScale = XMFLOAT3(scale, scale, 1.0f);
								break;
							case Terrain::negZ:
								instanceScale = XMFLOAT3(scale, -scale, 1.0f);
								break;
							}

							instanceVector.push_back(PatchInstanceData(instancePosition, instanceScale, XMFLOAT4(lod, 0.0f, 0.0f, 0.0f)));
						}
					}
				}
			}
		}

		nodeList.pop_front();
	}

	while(!lodStack.empty())
	{
		LODInfo* nextLOD = &lodStack.top();
		TraverseQuadTree(instanceVector, frustum, cameraPosition, XMLoadFloat3(&(nextLOD->offset)), nextLOD->scale, nextLOD->distance, lod + 1, face);

		lodStack.pop();
	}
}

void Terrain::PointToSphere(XMVECTOR& point, float radius)
{
	XMFLOAT3 vertexPosition;
	XMStoreFloat3(&vertexPosition, point);

	float xSQ = vertexPosition.x * vertexPosition.x;
	float ySQ = vertexPosition.y * vertexPosition.y;
	float zSQ = vertexPosition.z * vertexPosition.z;

	vertexPosition.x = vertexPosition.x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	vertexPosition.y = vertexPosition.y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	vertexPosition.z = vertexPosition.z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);

	point = XMLoadFloat3(&vertexPosition);
	point *= radius;
}

float Terrain::AABBDistance(float* extremities, const XMFLOAT3& position, XMFLOAT3* tolerance)
{
	if(tolerance)
	{
		// TODO: could probably figure out the direction of elevation from the centre position vector (if we don't end up doing the fractal here)
		extremities[0] += tolerance[0].x;
		extremities[2] += tolerance[0].y;
		extremities[4] += tolerance[0].z;

		extremities[1] += tolerance[1].x;
		extremities[3] += tolerance[1].y;
		extremities[5] += tolerance[1].z;
	}

	XMFLOAT3 distances(0.0f, 0.0f, 0.0f);
	if(position.x > extremities[0])
	{
		distances.x = position.x - extremities[0];
	}
	else if(position.x < extremities[1])
	{
		distances.x = extremities[1] - position.x;
	}

	if(position.y > extremities[2])
	{
		distances.y = position.y - extremities[2];
	}
	else if(position.y < extremities[3])
	{
		distances.y = extremities[3] - position.y;
	}

	if(position.z > extremities[4])
	{
		distances.z = position.z - extremities[4];
	}
	else if(position.z < extremities[5])
	{
		distances.z = extremities[5] - position.z;
	}

	float absDistance;
	XMStoreFloat(&absDistance, XMVector3Length(XMLoadFloat3(&distances)));

	return absDistance;
}

void Terrain::RenderGPUInstanceVector(const vector<PatchInstanceData>& instanceVector, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 cameraPosition)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	deviceContext->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	memcpy(mappedSubresource.pData, &instanceVector[0], sizeof(PatchInstanceData) * instanceVector.size());
	
	deviceContext->Unmap(instanceBuffer, 0);

	RenderGPUPatchInstances();

	gpuInstancedShader->RenderInstanced(deviceContext, instanceVector.size(), 8, 0);
}

void Terrain::RenderGPUPatchInstances()
{	
	// Set vertex buffer strides and offsets
	unsigned int strides[2] = { sizeof(Vertex), sizeof(PatchInstanceData) };
	unsigned int offsets[2] = { 0, 0 };
	ID3D11Buffer* bufferPointers[2] = { gpuVertexBuffer, instanceBuffer };
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);
}

void Terrain::RenderScreenQuad()
{
	unsigned int stride = sizeof(VertexTex);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &screenQuadVertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(screenQuadIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

void Terrain::EnableSphereCulling()
{
	useSphereCulling = true;
}

void Terrain::DisableSphereCulling()
{
	useSphereCulling = false;
}

void Terrain::EnableAABBCulling()
{
	useAABBCulling = true;
}

void Terrain::DisableAABBCulling()
{
	useAABBCulling = false;
}

void Terrain::EnableDistanceCulling()
{
	useDistanceCulling = true;
}

void Terrain::DisableDistanceCulling()
{
	useDistanceCulling = false;
}