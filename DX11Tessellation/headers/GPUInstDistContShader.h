#ifndef _GPUINSTDISTCONTSHADER_H_
#define _GPUINSTDISTCONTSHADER_H_

#include "shader.h"

class GPUInstDistContPerFrameParameter : public PerFrameParameterBase
{
public:
	GPUInstDistContPerFrameParameter(XMFLOAT4X4 viewMat, XMFLOAT4X4 projectionMat, XMFLOAT3 eyePos)
		: viewMatrix(viewMat), projectionMatrix(projectionMat), eyePosition(eyePos)
	{
	}
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT3 eyePosition;
};

class GPUInstDistContShader : public Shader
{
public:
	GPUInstDistContShader();

	bool SetPerFrameBuffers(ID3D11DeviceContext* context, PerFrameParameterBase* perFrameParameters);
	bool SetPerObjectBuffers(ID3D11DeviceContext* context, PerObjectParameterBase* perObjectParameters);
private:
	bool SetConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* context);
	bool SetInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer);
	bool ReleaseConstantBuffers();

	ID3D11Buffer* hullMatrixBuffer;
	ID3D11Buffer* domainMatrixBuffer;
	ID3D11Buffer* domainNoiseBuffer;
	ID3D11Buffer* pixelNoiseBuffer;
};

#endif