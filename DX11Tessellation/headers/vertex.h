#ifndef _VERTEX_H_
#define _VERTEX_H_

#include <xnamath.h>

struct Vertex
{
	XMFLOAT3 position;
};

struct VertexTex
{
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
};

#endif