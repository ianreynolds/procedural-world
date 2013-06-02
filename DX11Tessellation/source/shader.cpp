#include "shader.h"

Shader::Shader()
	: vertexShader(NULL), hullShader(NULL), domainShader(NULL), geometryShader(NULL), pixelShader(NULL), inputLayout(NULL)
{
}

Shader::~Shader()
{
}

bool Shader::Init(ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR vertexShaderFile, LPCWSTR hullShaderFile, LPCWSTR domainShaderFile, LPCWSTR geometryShaderFile, LPCWSTR pixelShaderFile)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* hullShaderBuffer;
	ID3D10Blob* domainShaderBuffer;
	ID3D10Blob* geometryShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	hullShaderBuffer = 0;
	domainShaderBuffer = 0;
	geometryShaderBuffer = 0;
	pixelShaderBuffer = 0;

	if(vertexShaderFile)
	{
		// Compile the vertex shader code.
		result = D3DX11CompileFromFile(vertexShaderFile, NULL, NULL, "main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
									   &vertexShaderBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				MessageBoxA(NULL, (LPCSTR)(errorMessage->GetBufferPointer()), "Vertex Shader Error", NULL);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(NULL, vertexShaderFile, L"Missing Vertex Shader File", MB_OK);
			}

			return false;
		}

		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
		if(FAILED(result))
		{
			return false;
		}
	}
	else
	{
		MessageBox(NULL, L"No vertex shader file specified.", L"Vertex Shader Error", NULL);
		return false;
	}

	if(hullShaderFile)
	{
		// Compile the hull shader code.
		result = D3DX11CompileFromFile(hullShaderFile, NULL, NULL, "main", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
									   &hullShaderBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				MessageBoxA(NULL, (LPCSTR)(errorMessage->GetBufferPointer()), "Hull Shader Error", NULL);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(NULL, hullShaderFile, L"Missing Hull Shader File", MB_OK);
			}

			return false;
		}

		// Create the hull shader from the buffer.
		result = device->CreateHullShader(hullShaderBuffer->GetBufferPointer(), hullShaderBuffer->GetBufferSize(), NULL, &hullShader);
		if(FAILED(result))
		{
			return false;
		}
	}

	if(domainShaderFile)
	{
		// Compile the domain shader code.
		result = D3DX11CompileFromFile(domainShaderFile, NULL, NULL, "main", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
									   &domainShaderBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				MessageBoxA(NULL, (LPCSTR)(errorMessage->GetBufferPointer()), "Domain Shader Error", NULL);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(NULL, domainShaderFile, L"Missing Domain Shader File", MB_OK);
			}

			return false;
		}

		// Create the domain shader from the buffer.
		result = device->CreateDomainShader(domainShaderBuffer->GetBufferPointer(), domainShaderBuffer->GetBufferSize(), NULL, &domainShader);
		if(FAILED(result))
		{
			return false;
		}
	}

	if(geometryShaderFile)
	{
		// Compile the geometry shader code.
		result = D3DX11CompileFromFile(geometryShaderFile, NULL, NULL, "main", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
									   &geometryShaderBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				MessageBoxA(NULL, (LPCSTR)(errorMessage->GetBufferPointer()), "Geometry Shader Error", NULL);
			}
			// If there was nothing in the error message then it simply could not find the shader file itself.
			else
			{
				MessageBox(NULL, geometryShaderFile, L"Missing Geometry Shader File", MB_OK);
			}

			return false;
		}

		// Create the geometry shader from the buffer.
		result = device->CreateGeometryShader(geometryShaderBuffer->GetBufferPointer(), geometryShaderBuffer->GetBufferSize(), NULL, &geometryShader);
		if(FAILED(result))
		{
			return false;
		}
	}

	if(pixelShaderFile)
	{
		// Compile the pixel shader code.
		result = D3DX11CompileFromFile(pixelShaderFile, NULL, NULL, "main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
									   &pixelShaderBuffer, &errorMessage, NULL);
		if(FAILED(result))
		{
			// If the shader failed to compile it should have writen something to the error message.
			if(errorMessage)
			{
				MessageBoxA(NULL, (LPCSTR)(errorMessage->GetBufferPointer()), "Pixel Shader Error", NULL);
			}
			// If there was nothing in the error message then it simply could not find the file itself.
			else
			{
				MessageBox(NULL, pixelShaderFile, L"Missing Pixel Shader File", MB_OK);
			}

			return false;
		}

		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
		if(FAILED(result))
		{
			return false;
		}
	}

	// Create the vertex input layout description.
	if(!SetInputLayout(device, vertexShaderBuffer))
	{
		return false;
	}
	
	// Release the shader buffers since they are no longer needed.
	if(vertexShaderBuffer)
	{
		vertexShaderBuffer->Release();
		vertexShaderBuffer = 0;
	}

	if(hullShaderBuffer)
	{
		hullShaderBuffer->Release();
		hullShaderBuffer = 0;
	}

	if(domainShaderBuffer)
	{
		domainShaderBuffer->Release();
		domainShaderBuffer = 0;
	}

	if(geometryShaderBuffer)
	{
		geometryShaderBuffer->Release();
		geometryShaderBuffer = 0;
	}

	if(pixelShaderBuffer)
	{
		pixelShaderBuffer->Release();
		pixelShaderBuffer = 0;
	}

	// Create the vertex input layout description.
	if(!SetConstantBuffers(device, context))
	{
		return false;
	}

	return true;
}

bool Shader::Render(ID3D11DeviceContext* context, unsigned int vertexCount, unsigned int vertexStart)
{
	context->Draw(vertexCount, vertexStart);

	return true;
}

bool Shader::RenderInstanced(ID3D11DeviceContext* context, int instanceCount, unsigned int vertexOffset, unsigned int instanceOffset)
{
	// Render the geometry
	context->DrawInstanced(4, instanceCount, vertexOffset, instanceOffset);
	
	return true;
}

bool Shader::RenderIndexed(ID3D11DeviceContext* context, int indexCount, unsigned int startIndex, unsigned int vertexOffset)
{
	// Render the geometry
	context->DrawIndexed(indexCount, startIndex, vertexOffset);
	
	return true;
}

bool Shader::Exit()
{
	if(vertexShader)
	{
		vertexShader->Release();
	}

	if(hullShader)
	{
		hullShader->Release();
	}

	if(domainShader)
	{
		domainShader->Release();
	}

	if(geometryShader)
	{
		geometryShader->Release();
	}

	if(pixelShader)
	{
		pixelShader->Release();
	}

	if(inputLayout)
	{
		inputLayout->Release();
	}

	ReleaseConstantBuffers();

	return true;
}