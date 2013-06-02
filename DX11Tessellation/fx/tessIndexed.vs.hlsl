cbuffer PerObjectBuffer : register(b0)
{
	float3 patchPosition;
	float3 patchScale;
	float4 lod;
}

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION0;
};

struct HullInputType
{
    float3 position : POSITION0;
	float lod : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HullInputType main(VertexInputType input)
{
    HullInputType output;
    
	// Pass the vertex position into the hull shader.
    output.position = (input.position * patchScale) + patchPosition;

	output.lod = lod.x;
    
    return output;
}