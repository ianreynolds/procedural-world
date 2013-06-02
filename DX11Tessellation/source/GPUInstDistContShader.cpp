#include "GPUInstDistContShader.h"

struct HullMatrixBuffer
{
	XMMATRIX view;
	XMMATRIX projection;
	XMVECTOR eyePosition;
};

struct DomainMatrixBuffer
{
	XMMATRIX view;
	XMMATRIX projection;
};

struct NoiseBuffer
{
	XMFLOAT4 pTable[512];
};

GPUInstDistContShader::GPUInstDistContShader()
	: Shader(), hullMatrixBuffer(nullptr), domainMatrixBuffer(nullptr), domainNoiseBuffer(nullptr),  pixelNoiseBuffer(nullptr)
{
}

bool GPUInstDistContShader::SetPerFrameBuffers(ID3D11DeviceContext* context, PerFrameParameterBase* perFrameParameters)
{
	// Set the type of primitive that should be rendered from this vertex buffer.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	// SET PARAMETERS
	
	GPUInstDistContPerFrameParameter* parameters = reinterpret_cast<GPUInstDistContPerFrameParameter*>(perFrameParameters);
	
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	HullMatrixBuffer* hullMatrixData;
	DomainMatrixBuffer* domainMatrixData;
	
	// Transpose the matrices to prepare them for the shader.
	XMMATRIX vMatrix = XMMatrixTranspose(XMLoadFloat4x4(&parameters->viewMatrix));
	XMMATRIX pMatrix = XMMatrixTranspose(XMLoadFloat4x4(&parameters->projectionMatrix));
	XMVECTOR eyePos = XMLoadFloat3(&parameters->eyePosition);
	
	// Lock the hull constant buffer so it can be written to.
	result = context->Map(hullMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the hull constant buffer.
	hullMatrixData = (HullMatrixBuffer*)mappedResource.pData;
	
	// Copy the hull data into the constant buffer.
	hullMatrixData->view = vMatrix;
	hullMatrixData->projection = pMatrix;
	hullMatrixData->eyePosition = eyePos;
	
	// Unlock the hull constant buffer.
	context->Unmap(hullMatrixBuffer, 0);
	
	// Set the position of the hull constant buffer in the hull shader.
	bufferNumber = 0;
	
	// Now set the hull constant buffer in the hull shader with the updated values.
	context->HSSetConstantBuffers(bufferNumber, 1, &hullMatrixBuffer);
	
	// Lock the domain constant buffer so it can be written to.
	result = context->Map(domainMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	
	// Get a pointer to the data in the domain constant buffer.
	domainMatrixData = (DomainMatrixBuffer*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	domainMatrixData->view = vMatrix;
	domainMatrixData->projection = pMatrix;
	
	// Unlock the domain constant buffer.
	context->Unmap(domainMatrixBuffer, 0);
	
	// Set the position of the domain constant buffer in the domain shader.
	bufferNumber = 0;
	
	// Finally set the domain constant buffer in the domain shader with the updated values.
	context->DSSetConstantBuffers(bufferNumber, 1, &domainMatrixBuffer);
	
	// RENDER
	
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

bool GPUInstDistContShader::SetPerObjectBuffers(ID3D11DeviceContext* context, PerObjectParameterBase* perObjectParameters)
{
	return true;
};

bool GPUInstDistContShader::SetConstantBuffers(ID3D11Device* device, ID3D11DeviceContext* context)
{
	HRESULT result;

	D3D11_BUFFER_DESC hullMatrixBufferDesc;
	D3D11_BUFFER_DESC domainMatrixBufferDesc;
	D3D11_BUFFER_DESC domainNoiseBufferDesc;
	D3D11_BUFFER_DESC pixelNoiseBufferDesc;

    // Setup the description of the matrix buffer in the hull shader
    hullMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	hullMatrixBufferDesc.ByteWidth = sizeof(HullMatrixBuffer);
    hullMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    hullMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hullMatrixBufferDesc.MiscFlags = 0;
	hullMatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the hull shader matrix buffer from within this class.
	result = device->CreateBuffer(&hullMatrixBufferDesc, NULL, &hullMatrixBuffer);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed to create hull matrix buffer", L"Constant Buffer Error", MB_OK);
		return false;
	}

	// Setup the description of the matrix buffer in the domain shader
    domainMatrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	domainMatrixBufferDesc.ByteWidth = sizeof(DomainMatrixBuffer);
    domainMatrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    domainMatrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    domainMatrixBufferDesc.MiscFlags = 0;
	domainMatrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the domain shader matrix buffer from within this class.
	result = device->CreateBuffer(&domainMatrixBufferDesc, NULL, &domainMatrixBuffer);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed to create domain matrix buffer", L"Constant Buffer Error", MB_OK);
		return false;
	}

	// Setup the description of the noise buffer in the domain shader
	domainNoiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	domainNoiseBufferDesc.ByteWidth = sizeof(NoiseBuffer);
	domainNoiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	domainNoiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	domainNoiseBufferDesc.MiscFlags = 0;
	domainNoiseBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the domain shader noise buffer from within this class.
	result = device->CreateBuffer(&domainNoiseBufferDesc, NULL, &domainNoiseBuffer);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed to create domain noise buffer", L"Constant Buffer Error", MB_OK);
		return false;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	NoiseBuffer* domainNoiseData;

	// Lock the domain noise buffer so it can be written to.
	result = context->Map(domainNoiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	
	// Get a pointer to the data in the domain constant buffer.
	domainNoiseData = (NoiseBuffer*)mappedResource.pData;

	static float p[] =  { 151,160,137,91,90,15,
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
	
	// Copy the permutation table into the constant buffer.
	for(int i = 0; i < 512; ++i)
	{
		domainNoiseData->pTable[i].x = p[i];
	}
	
	// Unlock the domain constant buffer.
	context->Unmap(domainNoiseBuffer, 0);
	
	// Set the position of the domain constant buffer in the domain shader.
	bufferNumber = 2;
	
	// Finally set the domain constant buffer in the domain shader with the updated values.
	context->DSSetConstantBuffers(bufferNumber, 1, &domainNoiseBuffer);

	// Setup the description of the noise buffer in the pixel shader
	pixelNoiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pixelNoiseBufferDesc.ByteWidth = sizeof(NoiseBuffer);
	pixelNoiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pixelNoiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pixelNoiseBufferDesc.MiscFlags = 0;
	pixelNoiseBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader noise buffer from within this class.
	result = device->CreateBuffer(&pixelNoiseBufferDesc, NULL, &pixelNoiseBuffer);
	if(FAILED(result))
	{
		MessageBox(NULL, L"Failed to create pixel noise buffer", L"Constant Buffer Error", MB_OK);
		return false;
	}

	NoiseBuffer* pixelNoiseData;

	// Lock the pixel noise buffer so it can be written to.
	result = context->Map(pixelNoiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}
	
	// Get a pointer to the data in the pixel constant buffer.
	pixelNoiseData = (NoiseBuffer*)mappedResource.pData;
	
	// Copy the permutation table into the constant buffer.
	for(int i = 0; i < 512; ++i)
	{
		pixelNoiseData->pTable[i].x = p[i];
	}
	
	// Unlock the pixel constant buffer.
	context->Unmap(pixelNoiseBuffer, 0);
	
	// Set the position of the pixel constant buffer in the pixel shader.
	bufferNumber = 2;
	
	// Finally set the pixel constant buffer in the pixel shader with the updated values.
	context->PSSetConstantBuffers(bufferNumber, 1, &pixelNoiseBuffer);

	return true;
}

bool GPUInstDistContShader::SetInputLayout(ID3D11Device* device, ID3D10Blob* vertexShaderBuffer)
{
	D3D11_INPUT_ELEMENT_DESC polygonLayout[4];
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
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[1].InputSlot = 1;
	polygonLayout[1].AlignedByteOffset = 0;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[1].InstanceDataStepRate = 1;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 1;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1;

	polygonLayout[3].SemanticName = "TEXCOORD";
	polygonLayout[3].SemanticIndex = 2;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[3].InputSlot = 1;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[3].InstanceDataStepRate = 1;

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

bool GPUInstDistContShader::ReleaseConstantBuffers()
{
	if(hullMatrixBuffer)
	{
		hullMatrixBuffer->Release();
	}

	if(domainMatrixBuffer)
	{
		domainMatrixBuffer->Release();
	}

	return true;
}