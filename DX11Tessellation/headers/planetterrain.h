#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <vector>

#include <D3DX11.h>
#include "objectcomponent.h"
#include "GPUInstDistContShader.h"
#include "LightingPassShader.h"
#include "RenderTexture.h"

using namespace std;

struct PatchInstanceData
{
	XMFLOAT3 instancePosition;
	XMFLOAT3 instanceScale;
	XMFLOAT4 instanceLODs;

	PatchInstanceData(XMFLOAT3 position, XMFLOAT3 scale, XMFLOAT4 lods)
		: instancePosition(position), instanceScale(scale), instanceLODs(lods)
	{
	}
};

struct PlanetTerrainQuadTreeNode
{
	PlanetTerrainQuadTreeNode()
		: size(0.0f)
	{
		childNodes[0] = NULL;
		childNodes[1] = NULL;
		childNodes[2] = NULL;
		childNodes[3] = NULL;
	}

	XMFLOAT3 centre;
	float size;
	PlanetTerrainQuadTreeNode* childNodes[4];
};

class Camera;
struct FrustumPlanes;
class D3D11Renderer;

class PlanetTerrain : public ObjectComponent
{
public:
	PlanetTerrain(ID3D11Device* device, ID3D11DeviceContext* context, D3D11Renderer* renderer, Shader* instancedGPUTerrainShader, Shader* lightingShader, RenderTexture* posBufferRenderTexture);
	~PlanetTerrain();

	enum CubeFace { posX, negX, posY, negY, posZ, negZ };

	void Render(Camera* camera, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);
	void RenderGPUInstanceVector(const vector<PatchInstanceData>& instanceVector, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT3 cameraPosition);
	void RenderGPUPatchInstances();
	void RenderScreenQuad();

	void CopyQuadTree(PlanetTerrainQuadTreeNode* destTree, PlanetTerrainQuadTreeNode* sourceTree, XMFLOAT4X4 transformMatrix);
	void TraverseQuadTree(vector<PatchInstanceData>& instanceVector, const FrustumPlanes& frustum, const XMVECTOR& cameraPosition, const XMVECTOR& offset, float scale, float lodDistance, int lod, CubeFace face);
	void PointToSphere(XMVECTOR& point, float radius);
	float AABBDistance(float* extremities, const XMFLOAT3& position, XMFLOAT3* tolerance);

	void EnableSphereCulling();
	void DisableSphereCulling();
	void EnableAABBCulling();
	void DisableAABBCulling();
	void EnableDistanceCulling();
	void DisableDistanceCulling();
private:
	bool useSphereCulling;
	bool useAABBCulling;
	bool useDistanceCulling;
	// TODO: shouldn't need to hold the device long-term
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	D3D11Renderer* d3d11Renderer;

	Shader* gpuInstancedShader;
	Shader* lightingPassShader;
	RenderTexture* positionBufferRenderTexture;

	ID3D11Buffer* screenQuadVertexBuffer;
	ID3D11Buffer* screenQuadIndexBuffer;

	ID3D11Buffer* gpuVertexBuffer;
	ID3D11Buffer* instanceBuffer;
};

#endif