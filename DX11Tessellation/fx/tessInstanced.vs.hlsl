
//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float3 position : POSITION0;
	float3 patchPosition : TEXCOORD0;
	float3 patchScale : TEXCOORD1;
	float4 lod : TEXCOORD2;
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
    output.position = (input.position * input.patchScale) + input.patchPosition;

	output.lod = input.lod.x;
    
    return output;
}