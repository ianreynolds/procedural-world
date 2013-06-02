cbuffer MatrixBuffer : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
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

	output.spherePosition = normalize(input.position);

	output.position = float4(input.position, 1.0f);
    
	// Pass the vertex position
	output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	
    return output;
}