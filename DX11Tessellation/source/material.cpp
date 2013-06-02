#include "material.h"

Material::Material()
{
	renderMaterial.Ambient.r = 1.0f;
	renderMaterial.Ambient.g = 1.0f;
	renderMaterial.Ambient.b = 1.0f;
	renderMaterial.Ambient.a = 1.0f;

	renderMaterial.Emissive.r = 0.0f;
	renderMaterial.Emissive.g = 0.0f;
	renderMaterial.Emissive.b = 0.0f;
	renderMaterial.Emissive.a = 0.0f;

	renderMaterial.Diffuse.r = 1.0f;
	renderMaterial.Diffuse.g = 1.0f;
	renderMaterial.Diffuse.b = 1.0f;
	renderMaterial.Diffuse.a = 1.0f;

	renderMaterial.Specular.r = 1.0f;
	renderMaterial.Specular.g = 1.0f;
	renderMaterial.Specular.b = 1.0f;
	renderMaterial.Specular.a = 1.0f;

	renderMaterial.Power = 100.0f;
}

Material::Material(ID3DXEffect* _effect, D3DXHANDLE* _effectTechnique)
{
	renderMaterial.Ambient.r = 1.0f;
	renderMaterial.Ambient.g = 1.0f;
	renderMaterial.Ambient.b = 1.0f;
	renderMaterial.Ambient.a = 1.0f;

	renderMaterial.Emissive.r = 0.0f;
	renderMaterial.Emissive.g = 0.0f;
	renderMaterial.Emissive.b = 0.0f;
	renderMaterial.Emissive.a = 0.0f;

	renderMaterial.Diffuse.r = 1.0f;
	renderMaterial.Diffuse.g = 1.0f;
	renderMaterial.Diffuse.b = 1.0f;
	renderMaterial.Diffuse.a = 1.0f;

	renderMaterial.Specular.r = 1.0f;
	renderMaterial.Specular.g = 1.0f;
	renderMaterial.Specular.b = 1.0f;
	renderMaterial.Specular.a = 1.0f;

	renderMaterial.Power = 100.0f;

	effect = _effect;
	effectTechnique = _effectTechnique;
}

Material::~Material()
{
}