#pragma once
#include "core/Base.h"
#include "renderer/Material.h"
#include "renderer/CubeMap.h"

class CityMaterial : public Material {
public:
	CityMaterial(Shader shader, std::string texture);
	virtual void Bind(Transform* transform) override;
private:
	Ref<Texture> m_BaseMap;
};
