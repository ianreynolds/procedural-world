#ifndef _LIGHTINGPASSSHADER_H_
#define _LIGHTINGPASSSHADER_H_

#include "shader.h"

class LightingPassPerFrameParameter : public PerFrameParameterBase
{
public:
	LightingPassPerFrameParameter(ID3D11ShaderResourceView* posBuffer)
		: positionBuffer(posBuffer)
	{
	}
	ID3D11ShaderResourceView* positionBuffer;
};

class LightingPassShader : public Shader
{
public:
	LightingPassShader();

	bool SetPerFrameBuffers(ID3D11DeviceContext* context, PerFrameParameterBase* perFrameParameters);
	bool SetPerObjectBuffers(ID3D11DeviceContext* context, PerObjectParameterBase* perObjectParameters);
private:
	bool SetConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* context);
	bool SetInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer);
	bool ReleaseConstantBuffers();

	ID3D11Buffer* pixelTextureBuffer;
};

#endif