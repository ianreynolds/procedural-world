#include "LightingPassShader.h"

struct VertexMatrixBuffer
{
	XMMATRIX view;
	XMMATRIX projection;
};

struct NoiseBuffer
{
	XMFLOAT4 pTable[512];
};

LightingPassShader::LightingPassShader()
	: Shader(), pixelTextureBuffer(nullptr)
{
}

bool LightingPassShader::SetPerFrameBuffers(ID3D11DeviceContext* context, PerFrameParameterBase* perFrameParameters)
{
	// Set the type of primitive that should be rendered from this vertex buffer.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// SET PARAMETERS
	LightingPassPerFrameParameter* parameters = reinterpret_cast<LightingPassPerFrameParameter*>(perFrameParameters);
	
	// set shader texture resources
	context->PSSetShaderResources(0, 1, &parameters->positionBuffer);
	
	// Set the vertex input layout
	context->IASetInputLayout(inputLayout);
	
	// Set the vertex, hull, domain, geometry and pixel shaders that will be used to render this geometry
	context->VSSetShader(vertexShader, NULL, 0);
	context->HSSetShader(hullShader, NULL, 0);
	context->DSSetShader(domainShader, NULL, 0);
	context->GSSetShader(geometryShader, NULL, 0);
	context->PSSetShader(pixelShader, NULL, 0);

	return true;
}

bool LightingPassShader::SetPerObjectBuffers(ID3D11DeviceContext* context, PerObjectParameterBase* perObjectParameters)
{
	return true;
};

bool LightingPassShader::SetConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* context)
{
	return true;
}

bool LightingPassShader::SetInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	// Create the vertex input layout description.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	HRESULT result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &inputLayout);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed to create input layout", L"Input Layout Error", MB_OK);
		return false;
	}

	return true;
}

bool LightingPassShader::ReleaseConstantBuffers()
{
	if(pixelTextureBuffer)
	{
		pixelTextureBuffer->Release();
	}

	return true;
}