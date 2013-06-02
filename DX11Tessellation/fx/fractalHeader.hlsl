cbuffer NoiseBuffer : register(b2)
{
	float4 pTable[512];
};

float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float fadeD(float t)
{
	return 30 * t * t * (t * (t - 2) + 1);
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

float perlinNoise(float x, float y, float z)
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

	int A = pTable[X].x+Y;										// HASH COORDINATES OF
	int AA = pTable[A].x+Z;											// THE 8 CUBE CORNERS,
	int AB = pTable[A+1].x+Z;
	int B = pTable[X+1].x+Y;
	int BA = pTable[B].x+Z;
	int BB = pTable[B+1].x+Z;

	return lerp(w, lerp(v, lerp(u, grad(pTable[AA].x, x, y, z),		// AND ADD
						grad(pTable[BA].x, x-1, y, z)),				// BLENDED
					lerp(u, grad(pTable[AB].x, x, y-1, z),				// RESULTS
						grad(pTable[BB].x, x-1, y-1, z))),				// FROM  8
					lerp(v, lerp(u, grad(pTable[AA+1].x, x, y, z-1),	// CORNERS
						grad(pTable[BA+1].x, x-1, y, z-1)),			// OF CUBE
					lerp(u, grad(pTable[AB+1].x, x, y-1, z-1),
						grad(pTable[BB+1].x, x-1, y-1, z-1))));
}

float perlinNoiseD(float3 inPoint, out float3 gradient)
{
	int X = (int)floor(inPoint.x) & 255;							// FIND UNIT CUBE THAT
	int Y = (int)floor(inPoint.y) & 255;							// CONTAINS POINT.
	int Z = (int)floor(inPoint.z) & 255;

	inPoint.x -= floor(inPoint.x);											// FIND RELATIVE X,Y,Z
	inPoint.y -= floor(inPoint.y);											// OF POINT IN CUBE.
	inPoint.z -= floor(inPoint.z);

	float u = fade(inPoint.x);											// COMPUTE FADE CURVES
	float v = fade(inPoint.y);											// FOR EACH OF X,Y,Z.
	float w = fade(inPoint.z);

	float du = fadeD(inPoint.x);
	float dv = fadeD(inPoint.y);
	float dw = fadeD(inPoint.z);

	int A = pTable[X].x+Y;										// HASH COORDINATES OF
	int AA = pTable[A].x+Z;											// THE 8 CUBE CORNERS,
	int AB = pTable[A+1].x+Z;
	int B = pTable[X+1].x+Y;
	int BA = pTable[B].x+Z;
	int BB = pTable[B+1].x+Z;

	float a = grad(pTable[AA].x, inPoint.x, inPoint.y, inPoint.z);
	float b = grad(pTable[BA].x, inPoint.x-1, inPoint.y, inPoint.z);
	float c = grad(pTable[AB].x, inPoint.x, inPoint.y-1, inPoint.z);
	float d = grad(pTable[BB].x, inPoint.x-1, inPoint.y-1, inPoint.z);
	float e = grad(pTable[AA+1].x, inPoint.x, inPoint.y, inPoint.z-1);
	float f = grad(pTable[BA+1].x, inPoint.x-1, inPoint.y, inPoint.z-1);
	float g = grad(pTable[AB+1].x, inPoint.x, inPoint.y-1, inPoint.z-1);
	float h = grad(pTable[BB+1].x, inPoint.x-1, inPoint.y-1, inPoint.z-1);

	float ce[8] = 
	{
		a,
		e - a,
		c - a,
		b - a,
		a + g - c - e,
		a + f - b - e,
		a + d - b - c,
		b + c + e + h - a - d - f - g
	};

	gradient.x = du * v * w * ce[7] + du * v * ce[6] + du * w * ce[5] + du * ce[3];
	gradient.y = dv * u * w * ce[7] + dv * u * ce[6] + dv * w * ce[4] + dv * ce[2];
	gradient.z = dw * u * v * ce[7] + dw * u * ce[5] + dw * v * ce[4] + dw * ce[1];

	return u * v * w * ce[7] + u * v * ce[6] + u * w * ce[5] + v * w * ce[4] + u * ce[3] + v * ce[2] + w * ce[1] + ce[0];
}

float RidgedMultifractal(float3 inPoint)
{
	float H = 1.0f;
	float lacunarity = 2.0f;
	float octaves = 6.0f;
	float offset = 1.0f;
	float gain = 2.0f;
	float exponentArray[20];
	
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
	signal = perlinNoise(inPoint.x, inPoint.y, inPoint.z);
	
	// get absolute value of signal (this creates the ridges)
	if ( signal < 0.0 ) 
		signal = -signal;
	
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
		inPoint *= lacunarity;

		// weight successive contributions by previous signal
		weight = signal * gain;
		if(weight > 1.0)
			weight = 1.0;
		if(weight < 0.0)
			weight = 0.0;
		signal = perlinNoise(inPoint.x, inPoint.y, inPoint.z);
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

float fBm(float3 inPoint)
{
	float H = 0.9f;
	float lacunarity = 2.0f;
	float octaves = 8.0f;

	float value = 0.0;

	/* inner loop of fractal construction */
	for(int i=0; i < octaves; i++)
	{
		value += perlinNoise(inPoint.x, inPoint.y, inPoint.z) * pow(lacunarity, -H * i);
		inPoint *= lacunarity;
	}

	float remainder = octaves - int(octaves);
	if (remainder) /* add in “octaves” remainder */
		/* ‘i’ and spatial freq. are preset in loop above */
		value += remainder * perlinNoise(inPoint.x, inPoint.y, inPoint.z) * pow(lacunarity, -H * i);

	return value;
}