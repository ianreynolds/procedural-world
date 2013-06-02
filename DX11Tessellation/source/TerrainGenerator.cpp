#include "TerrainGenerator.h"

static float pTable[] =  { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
		151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float grad(int hash, float x, float y, float z)
{
    int h = hash & 15;										// CONVERT LO 4 BITS OF HASH CODE
    float u = h<8 ? x : y;									// INTO 12 GRADIENT DIRECTIONS.
    float v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

float TerrainGenerator::perlinNoise(float x, float y, float z)
{
	int X = (int)floor(x) & 255;							// FIND UNIT CUBE THAT
	int Y = (int)floor(y) & 255;							// CONTAINS POINT.
	int Z = (int)floor(z) & 255;

	x -= floor(x);											// FIND RELATIVE X,Y,Z
	y -= floor(y);											// OF POINT IN CUBE.
	z -= floor(z);

	float u = fade(x);											// COMPUTE FADE CURVES
	float v = fade(y);											// FOR EACH OF X,Y,Z.
	float w = fade(z);

	int A = pTable[X]+Y;										// HASH COORDINATES OF
	int AA = pTable[A]+Z;											// THE 8 CUBE CORNERS,
	int AB = pTable[A+1]+Z;
	int B = pTable[X+1]+Y;
	int BA = pTable[B]+Z;
	int BB = pTable[B+1]+Z;

	return lerp(w, lerp(v, lerp(u, grad(pTable[AA], x, y, z),		// AND ADD
						grad(pTable[BA], x-1, y, z)),				// BLENDED
					lerp(u, grad(pTable[AB], x, y-1, z),				// RESULTS
						grad(pTable[BB], x-1, y-1, z))),				// FROM  8
					lerp(v, lerp(u, grad(pTable[AA+1], x, y, z-1),	// CORNERS
						grad(pTable[BA+1], x-1, y, z-1)),			// OF CUBE
					lerp(u, grad(pTable[AB+1], x, y-1, z-1),
						grad(pTable[BB+1], x-1, y-1, z-1))));
}

float TerrainGenerator::RidgedMultifractal(XMFLOAT3 inPoint)
{
	float H = 1.0f;
	float lacunarity = 2.0f;
	float octaves = 6.0f;
	float offset = 1.0f;
	float gain = 2.0f;
	float exponentArray[12];
	
	float result = 0.0f;
	float frequency = 1.0f;
	float signal = 0.0f;
	float weight = 0.0f;
	int i;

	for(i = 0; i < octaves; i++)
	{
		// compute weight for each frequency
		exponentArray[i] = pow(frequency, -H);
		frequency *= lacunarity;
	}

	frequency = 0.0f;
	
	// get first octave
	signal = abs(TerrainGenerator::perlinNoise(inPoint.x, inPoint.y, inPoint.z));
	
	// get absolute value of signal (this creates the ridges)
	//if ( signal < 0.0 ) 
	//	signal = -signal;
	
	// invert and translate (note that "offset should be ~ = 1.0)
	signal = offset - signal;

	// square the signal, to increase “sharpness” of ridges
	signal *= signal;

	// assign initial values
	result = signal;
	weight = 1.0;
	for(i = 0; i < octaves; i++)
	{
		// increase the frequency
		inPoint.x *= lacunarity;
		inPoint.y *= lacunarity;
		inPoint.z *= lacunarity;

		// weight successive contributions by previous signal
		weight = signal * gain;
		if(weight > 1.0)
			weight = 1.0;
		if(weight < 0.0)
			weight = 0.0;
		signal = TerrainGenerator::perlinNoise(inPoint.x, inPoint.y, inPoint.z);
		if (signal < 0.0)
			signal = -signal;
		signal = offset - signal;
		signal *= signal;
		
		// weight the contribution
		signal *= weight;
		result += signal * exponentArray[i];
	}

	return result;
}

float TerrainGenerator::fBm(XMFLOAT3 inPoint)
{
	float H = 0.9f;
	float lacunarity = 2.0f;
	float octaves = 16.0f;

	float value = 0.0;
	int i;

	/* inner loop of fractal construction */
	for(i = 0; i < octaves; i++)
	{
		value += TerrainGenerator::perlinNoise(inPoint.x, inPoint.y, inPoint.z) * pow(lacunarity, -H * i);
		inPoint.x *= lacunarity;
		inPoint.y *= lacunarity;
		inPoint.z *= lacunarity;
	}

	float remainder = octaves - int(octaves);
	if (remainder) /* add in “octaves” remainder */
		/* ‘i’ and spatial freq. are preset in loop above */
		value += remainder * TerrainGenerator::perlinNoise(inPoint.x, inPoint.y, inPoint.z) * pow(lacunarity, -H * i);

	return value;
}

XMFLOAT3 TerrainGenerator::GetContinentPoint(const XMFLOAT3& inPoint)
{
	float fBmScale = 0.95547f;

	XMFLOAT3 fBmPoint;
	fBmPoint.x = inPoint.x / fBmScale;
	fBmPoint.y = inPoint.y / fBmScale;
	fBmPoint.z = inPoint.z / fBmScale;

	float fBmPos = max(fBm(fBmPoint), 0.0f);

	float multifractalScale = 0.0095547f;

	XMFLOAT3 multifractalPoint;
	multifractalPoint.x = inPoint.x / multifractalScale;
	multifractalPoint.y = inPoint.y / multifractalScale;
	multifractalPoint.z = inPoint.z / multifractalScale;

	float multifractalPos = RidgedMultifractal(multifractalPoint);

	float continentValue = 6371.0f + (multifractalPos * fBmPos * 2.0f + fBmPos * 3.0f) * 7.2f;
	

	//float noiseScale = 0.0095547f;

	//XMFLOAT3 noisePoint;
	//noisePoint.x = inPoint.x / noiseScale;
	//noisePoint.y = inPoint.y / noiseScale;
	//noisePoint.z = inPoint.z / noiseScale;

	//float noisePos = max(perlinNoise(noisePoint.x, noisePoint.y, noisePoint.z), 0.0f);
	
	//float continentValue = 6371.0f + noiseScale * 7.2f;	


	XMFLOAT3 outPoint = inPoint;
	outPoint.x *= continentValue;
	outPoint.y *= continentValue;
	outPoint.z *= continentValue;

	return outPoint;
}