#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <d3dx9.h>

#include "objectcomponent.h"
#include "transform.h"

class Material : public ObjectComponent
{
public:
	Material();
	Material(ID3DXEffect* _effect, D3DXHANDLE* _effectTechnique);
	virtual ~Material();

	void SetRenderMaterial(D3DMATERIAL9 mat);

	virtual void StartEffect() = 0;
	virtual void EndEffect() = 0;
protected:
	virtual void UpdateEffectParameters() = 0;

	ID3DXEffect* effect;
	D3DXHANDLE* effectTechnique;

	D3DMATERIAL9 renderMaterial;
};

inline void Material::SetRenderMaterial(D3DMATERIAL9 mat)
{
	renderMaterial = mat;
}

#endif