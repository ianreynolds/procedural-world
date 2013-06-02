#include "fractalHeader.hlsl"

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 spherePosition : TEXCOORD0;
	float3 worldPosition : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//float3 lightPosition = float3(0.0f, 8000.0f, 0.0f);
	//outColor = saturate(max(dot(input.normal, normalize(lightPosition - input.worldPosition)), 0.0f));

	float3 spherePosition = normalize(input.spherePosition);

	//////////////////////////////////////////  Continent Generation  /////////////////////////////////////////////////
	float3 tangentX = normalize(ddx(input.spherePosition));
	float3 tangentY = normalize(ddy(input.spherePosition));

	float fBmScale = 0.95547f;
	float fBmPos = max(fBm(input.spherePosition / fBmScale), 0.0f);

	float multifractalScale = 0.0095547f;
	float multifractalPos = RidgedMultifractal(spherePosition / multifractalScale);

	float terrainHeight = multifractalPos * fBmPos * 2.0f + fBmPos * 3.0f;
	float3 terrainPosition = spherePosition * (6371.0f + terrainHeight * 7.2f);

	return float4(terrainPosition, terrainHeight);
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////  Vanilla Noise  ////////////////////////////////////////////////////////
	//float noiseScale = 0.0095547f;
	////float noisePos = max(perlinNoise(input.spherePosition.x / noiseScale, input.spherePosition.y / noiseScale, input.spherePosition.z / noiseScale), 0.0f);
	//float3 gradient;
	//float noisePos = max(perlinNoiseD(input.spherePosition / noiseScale, gradient), 0.0f);
//
	//float terrainHeight = noisePos * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////  fBm  //////////////////////////////////////////////////////////////////
	//float fBmScale = 0.0095547f;
	//float fBmPos = max(fBm(input.spherePosition / fBmScale), 0.0f);
//
	//float terrainHeight = fBmPos * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////  Ridged Multifractal  //////////////////////////////////////////////////
	//float multifractalScale = 0.0095547f;
	//float multifractalPos = max(RidgedMultifractal(input.spherePosition / multifractalScale), 0.0f);
//
	//float terrainHeight = multifractalPos * 7.2f;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float4 noiseColor;

	if(terrainHeight <= 0.0f)
		noiseColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if(terrainHeight <= 0.5f)
		noiseColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if(terrainHeight <= 0.75f)
		noiseColor = float4(0.7f, 0.7f, 0.7f, 1.0f);
	else if(terrainHeight <= 1.75f)
		noiseColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	else
		noiseColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 outColor;

	float3 normal = normalize(cross(ddx(input.worldPosition), ddy(input.worldPosition)));
	//float3 normal = normalize(normalize(input.spherePosition) - normalize(gradient));
	//float3 normal = normalize(gradient + float3(1.0f, 1.0f, 1.0f));
	float3 lightPosition = float3(0.0f, 8000.0f, 0.0f);
	outColor = saturate(max(dot(normal, normalize(lightPosition - input.worldPosition)), 0.25f));

	return noiseColor * outColor;
}