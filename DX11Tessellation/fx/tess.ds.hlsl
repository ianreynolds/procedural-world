/////////////
// GLOBALS //
/////////////

#include "fractalHeader.hlsl"
#include "sphereHeader.hlsl"

cbuffer MatrixBuffer : register(b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullOutputType
{
    float3 position : POSITION0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 spherePosition : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Domain Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]

PixelInputType main(ConstantOutputType input, float2 uvCoord : SV_DomainLocation, const OutputPatch<HullOutputType, 4> patch)
{
	float3 vertexPosition;
	PixelInputType output;
 
 	// Determine the position of the new vertex.
	vertexPosition = patch[0].position + uvCoord.x * (patch[1].position - patch[0].position) + uvCoord.y * (patch[3].position - patch[0].position);

	// Transform into sphere mapped coordinates
	vertexPosition = SphereMapPoint(vertexPosition);

	//vertexPosition *= 4000.0f;
	output.spherePosition = vertexPosition;

	//////////////////////////////////////////  Continent Generation  /////////////////////////////////////////////////
	float fBmScale = 0.95547f;
	float fBmPos = max(fBm(vertexPosition / fBmScale), 0.0f);

	float multifractalScale = 0.0095547f;
	float multifractalPos = RidgedMultifractal(vertexPosition / multifractalScale);

	vertexPosition *= 6371.0f + (multifractalPos * fBmPos * 2.0f + fBmPos * 3.0f) * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////  Vanilla Noise  ////////////////////////////////////////////////////////
	//float noiseScale = 0.0095547f;
	//float noisePos = max(perlinNoise(vertexPosition.x / noiseScale, vertexPosition.y / noiseScale, vertexPosition.z / noiseScale), 0.0f);
//
	//vertexPosition *= 6371.0f + noisePos * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////  fBm  //////////////////////////////////////////////////////////////////
	//float fBmScale = 0.0095547f;
	//float fBmPos = max(fBm(vertexPosition / fBmScale), 0.0f);
//
	//vertexPosition *= 6371.0f + fBmPos * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////  Ridged Multifractal  //////////////////////////////////////////////////
	//float multifractalScale = 0.0095547f;
	//float multifractalPos = max(RidgedMultifractal(vertexPosition / multifractalScale), 0.0f);
//
	//vertexPosition *= 6371.0f + multifractalPos * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	output.worldPosition = vertexPosition;
	output.position = float4(vertexPosition, 1.0f);

    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    return output;
}