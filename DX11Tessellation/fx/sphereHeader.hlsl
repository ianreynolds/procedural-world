float3 SphereMapPoint(float3 cubePosition)
{
	float3 vertexPosition;

	float xSQ = cubePosition.x * cubePosition.x;
	float ySQ = cubePosition.y * cubePosition.y;
	float zSQ = cubePosition.z * cubePosition.z;

	vertexPosition.x = cubePosition.x * sqrt(1.0f - ySQ / 2.0f - zSQ / 2.0f + (ySQ * zSQ) / 3.0f);
	vertexPosition.y = cubePosition.y * sqrt(1.0f - xSQ / 2.0f - zSQ / 2.0f + (xSQ * zSQ) / 3.0f);
	vertexPosition.z = cubePosition.z * sqrt(1.0f - xSQ / 2.0f - ySQ / 2.0f + (xSQ * ySQ) / 3.0f);

	return vertexPosition;
}