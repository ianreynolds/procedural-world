#ifndef _TERRAINGENERATOR_H_
#define _TERRAINGENERATOR_H_

#include <d3dx11.h>
#include <xnamath.h>

class TerrainGenerator
{
public:
	static float perlinNoise(float x, float y, float z);
	static float RidgedMultifractal(XMFLOAT3 inPoint);
	static float fBm(XMFLOAT3 inPoint);

	static XMFLOAT3 GetContinentPoint(const XMFLOAT3& inPoint);
};

#endif