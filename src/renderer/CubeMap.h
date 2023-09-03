#pragma once
#include <array>
#include <unordered_map>
#include <string>
#include "core/Base.h"

class CubeMap {
public:
	static Ref<CubeMap> Load(const std::array<std::string, 6>& images);
	CubeMap() = delete;
	CubeMap(const std::string& key, u32 id);
	~CubeMap();
	void Bind();
private:
	inline static std::unordered_map<std::string, std::weak_ptr<CubeMap>> m_ActiveCubemaps;
	std::string m_Key;
	u32 m_Id;
};

