// TODO: use a buffer for this please...
Texture2D positionBuffer;
SamplerState SampleType;

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
	//return positionBuffer.Sample(SampleType, input.texCoord);

	float4 backColor = float4(0.0f, 0.125f, 0.3f, 1.0f);

	float4 worldPosition = positionBuffer.Sample(SampleType, input.texCoord);

	if(worldPosition.x == backColor.r && worldPosition.y == backColor.g && worldPosition.z == backColor.b)
	{
		return backColor;
	}
	else
	{
		float2 texelOffsets[4] = 
		{
			float2(1.0f / 800.0f, 0.0f),
			float2(-1.0f / 800.0f, 0.0f),
			float2(0.0f, 1.0f / 600.0f),
			float2(0.0f, -1.0f / 600.0f)
		};

		float3 deltaX = positionBuffer.Sample(SampleType, input.texCoord + texelOffsets[0]) - positionBuffer.Sample(SampleType, input.texCoord + texelOffsets[1]);
		float3 deltaY = positionBuffer.Sample(SampleType, input.texCoord + texelOffsets[2]) - positionBuffer.Sample(SampleType, input.texCoord + texelOffsets[3]);

		float4 noiseColor;
		float terrainHeight = worldPosition.w;

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

		float3 normal = normalize(cross(deltaX, deltaY));
		float3 lightPosition = float3(0.0f, 16000.0f, 0.0f);
		float4 outColor = saturate(max(dot(normal, normalize(lightPosition - worldPosition.xyz)), 0.25f));

		return noiseColor * outColor;
	}
}