#pragma once
#include "renderer/Material.h"
#include "renderer/CubeMap.h"

class CityMaterial : public Material {
public:
	CityMaterial(Shader shader);
	virtual void Bind(Transform* transform) override;
private:
	Texture baseMap;
};
