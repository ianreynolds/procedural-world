#ifndef _SHADER_H_
#define _SHADER_H_

#include <d3dx11.h>
#include <xnamath.h>

#include "vertex.h"

class PerFrameParameterBase
{
};

class PerObjectParameterBase
{
};

class Shader
{
public:
	Shader();
	~Shader();

	bool Init(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR vertexShaderFile, LPCWSTR hullShaderFile, LPCWSTR domainShaderFile, LPCWSTR geometryShaderFile, LPCWSTR pixelShaderFile);
	bool Exit();
	
	bool Render(ID3D11DeviceContext* context, unsigned int vertexCount, unsigned int vertexStart);
	bool RenderInstanced(ID3D11DeviceContext* context, int instanceCount, unsigned int vertexOffset, unsigned int instanceOffset);
	bool RenderIndexed(ID3D11DeviceContext* context, int indexCount, unsigned int startIndex, unsigned int vertexOffset);

	virtual bool SetPerFrameBuffers(ID3D11DeviceContext* context, PerFrameParameterBase* perFrameParameters) = 0;
	virtual bool SetPerObjectBuffers(ID3D11DeviceContext* context, PerObjectParameterBase* perObjectParameters) = 0;
protected:
	virtual bool SetConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* context) = 0;
	virtual bool SetInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer) = 0;
	virtual bool ReleaseConstantBuffers() = 0;
	ID3D11VertexShader* vertexShader;
	ID3D11HullShader* hullShader;
	ID3D11DomainShader* domainShader;
	ID3D11GeometryShader* geometryShader;
	ID3D11PixelShader* pixelShader;
	ID3D11InputLayout* inputLayout;
};

#endif