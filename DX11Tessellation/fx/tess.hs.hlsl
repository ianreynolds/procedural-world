/////////////
// GLOBALS //
/////////////
cbuffer TessellationBuffer : register (b0)
{
    matrix viewMatrix;
    matrix projectionMatrix;
	float4 eyePosition;
};

//////////////
// TYPEDEFS //
//////////////
struct HullInputType
{
    float3 position : POSITION0;
	float lod : TEXCOORD0;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct HullOutputType
{
    float3 position : POSITION0;
};

float LinePointDistance(float3 x0, float3 x1, float maxHeight);

////////////////////////////////////////////////////////////////////////////////
// Patch Constant Function
////////////////////////////////////////////////////////////////////////////////
ConstantOutputType PatchConstantFunction(InputPatch<HullInputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;

	//output.edges[0] = 2.0f;
	//output.edges[1] = 2.0f;
	//output.edges[2] = 2.0f;
	//output.edges[3] = 2.0f;
//
	//output.inside[0] = 2.0f;
	//output.inside[1] = 2.0f;
//
	//return output;

	float4 screenCoord[4];
	screenCoord[0] = float4(inputPatch[0].position, 1.0f);
	screenCoord[1] = float4(inputPatch[1].position, 1.0f);
	screenCoord[2] = float4(inputPatch[2].position, 1.0f);
	screenCoord[3] = float4(inputPatch[3].position, 1.0f);

	float xSQ, ySQ, zSQ;

	xSQ = screenCoord[0].x * screenCoord[0].x;
	ySQ = screenCoord[0].y * screenCoord[0].y;
	zSQ = screenCoord[0].z * screenCoord[0].z;
	screenCoord[0].x = screenCoord[0].x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	screenCoord[0].y = screenCoord[0].y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	screenCoord[0].z = screenCoord[0].z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);
	screenCoord[0].xyz *= 6371.0f;

	xSQ = screenCoord[1].x * screenCoord[1].x;
	ySQ = screenCoord[1].y * screenCoord[1].y;
	zSQ = screenCoord[1].z * screenCoord[1].z;
	screenCoord[1].x = screenCoord[1].x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	screenCoord[1].y = screenCoord[1].y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	screenCoord[1].z = screenCoord[1].z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);
	screenCoord[1].xyz *= 6371.0f;

	xSQ = screenCoord[2].x * screenCoord[2].x;
	ySQ = screenCoord[2].y * screenCoord[2].y;
	zSQ = screenCoord[2].z * screenCoord[2].z;
	screenCoord[2].x = screenCoord[2].x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	screenCoord[2].y = screenCoord[2].y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	screenCoord[2].z = screenCoord[2].z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);
	screenCoord[2].xyz *= 6371.0f;

	xSQ = screenCoord[3].x * screenCoord[3].x;
	ySQ = screenCoord[3].y * screenCoord[3].y;
	zSQ = screenCoord[3].z * screenCoord[3].z;
	screenCoord[3].x = screenCoord[3].x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	screenCoord[3].y = screenCoord[3].y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	screenCoord[3].z = screenCoord[3].z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);
	screenCoord[3].xyz *= 6371.0f;

	//////////////////////////////// DISTANCE ADAPTIVE ////////////////////////////////////////

	const float minLODDistance[3] = { 2400.0f, 600.0f, 40.0f };
	const float lodIncrement[3] = { 450.0f, 140.0f, 6.67f };

	int currentLOD = int(inputPatch[0].lod) - 1;
	const int maxLOD = 3;

	//TODO: these distances are all on the sphere surface, slight discontinuity with high range stuff...
	float edgeDistance[4];
	edgeDistance[0] = distance(eyePosition.xyz, lerp(screenCoord[3].xyz, screenCoord[0].xyz, 0.5f));
	edgeDistance[1] = distance(eyePosition.xyz, lerp(screenCoord[1].xyz, screenCoord[0].xyz, 0.5f));
	edgeDistance[2] = distance(eyePosition.xyz, lerp(screenCoord[2].xyz, screenCoord[1].xyz, 0.5f));
	edgeDistance[3] = distance(eyePosition.xyz, lerp(screenCoord[3].xyz, screenCoord[2].xyz, 0.5f));
	//const float maxHeight = 10.0f;
	//edgeDistance[0] = LinePointDistance(eyePosition.xyz, lerp(screenCoord[3].xyz, screenCoord[0].xyz, 0.5f), maxHeight);
	//edgeDistance[1] = LinePointDistance(eyePosition.xyz, lerp(screenCoord[1].xyz, screenCoord[0].xyz, 0.5f), maxHeight);
	//edgeDistance[2] = LinePointDistance(eyePosition.xyz, lerp(screenCoord[2].xyz, screenCoord[1].xyz, 0.5f), maxHeight);
	//edgeDistance[3] = LinePointDistance(eyePosition.xyz, lerp(screenCoord[3].xyz, screenCoord[2].xyz, 0.5f), maxHeight);

	float edges[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	edges[0] = pow(2, ceil(clamp((minLODDistance[currentLOD] - edgeDistance[0]) / lodIncrement[currentLOD], 0.0f, 6.0f)));
	edges[1] = pow(2, ceil(clamp((minLODDistance[currentLOD] - edgeDistance[1]) / lodIncrement[currentLOD], 0.0f, 6.0f)));
	edges[2] = pow(2, ceil(clamp((minLODDistance[currentLOD] - edgeDistance[2]) / lodIncrement[currentLOD], 0.0f, 6.0f)));
	edges[3] = pow(2, ceil(clamp((minLODDistance[currentLOD] - edgeDistance[3]) / lodIncrement[currentLOD], 0.0f, 6.0f)));

	float maxTessellation[3] = { 16.0f, 16.0f, 64.0f };
	output.edges[0] = min(edges[0], maxTessellation[currentLOD]);
	output.edges[1] = min(edges[1], maxTessellation[currentLOD]);
	output.edges[2] = min(edges[2], maxTessellation[currentLOD]);
	output.edges[3] = min(edges[3], maxTessellation[currentLOD]);

	float inner[2];

	inner[0] = (output.edges[1] + output.edges[3]) * 0.5f;
	inner[1] = (output.edges[0] + output.edges[2]) * 0.5f;

	for(int i = 0; i < 2; i++)
	{
		if(inner[i] > 32.0f)
		{
			inner[i] = 64.0f;
		}
		else if(inner[i] > 16.0f)
		{
			inner[i] = 32.0f;
		}
		else if(inner[i] > 8.0f)
		{
			inner[i] = 16.0f;
		}
		else if(inner[i] > 4.0f)
		{
			inner[i] = 8.0f;
		}
		else if(inner[i] > 2.0f)
		{
			inner[i] = 4.0f;
		}
		else if(inner[i] > 1.0f)
		{
			inner[i] = 2.0f;
		}
		else
		{
			inner[i] = 1.0f;
		}
	}

	output.inside[0] = inner[0];
	output.inside[1] = inner[1];

	return output;
}

////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]

HullOutputType main(InputPatch<HullInputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOutputType output;

	// Set the world position for this control point as the output position.
	output.position = patch[pointId].position;

    return output;
}

float LinePointDistance(float3 x0, float3 x1, float maxHeight)
{
	float distanceToLine;

	float3 x2 = x1 + normalize(x1) * maxHeight;

	distanceToLine = length(cross((x2 - x1), (x1 - x0))) / length(x2 - x1);

	return distanceToLine;
}