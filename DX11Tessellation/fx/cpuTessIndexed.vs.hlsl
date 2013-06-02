#include "fractalHeader.hlsl"

cbuffer MatrixBuffer : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
}

cbuffer PerObjectBuffer : register(b1)
{
	float3 patchPosition;
	float3 patchScale;
}

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
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
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    
	float3 vertexPosition = (input.position * patchScale) + patchPosition;

	////////////////////////// USING GEOMETRY
	float xSQ = vertexPosition.x * vertexPosition.x;
	float ySQ = vertexPosition.y * vertexPosition.y;
	float zSQ = vertexPosition.z * vertexPosition.z;

	vertexPosition.x = vertexPosition.x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	vertexPosition.y = vertexPosition.y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	vertexPosition.z = vertexPosition.z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);

	// pass the vertex position of the sphere surface
	output.spherePosition = vertexPosition;

	float fBmScale = 0.95547f;
	float fBmPos = max(fBm(vertexPosition / fBmScale), 0.0f);

	float multifractalScale = 0.0095547f;
	float multifractalPos = RidgedMultifractal(vertexPosition / multifractalScale);

	vertexPosition *= 6371.0f + (multifractalPos * fBmPos * 2.0f + fBmPos * 3.0f) * 7.2f;

	output.position = float4(vertexPosition, 1.0f);
	
	// Pass the vertex position
	output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	
    return output;
}