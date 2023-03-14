#pragma once
#include <array>
#include <cstdint>
#include <unordered_map>
#include <memory>
#include <string>

class CubeMap {
public:
	static std::shared_ptr<CubeMap> Load(const std::array<std::string, 6>& images);
	CubeMap() = delete;
	CubeMap(const std::string& key, uint32_t id);
	~CubeMap();
	void Bind();
private:
	static std::unordered_map<std::string, std::weak_ptr<CubeMap>> m_ActiveCubemaps;
	std::string m_Key;
	uint32_t m_Id;
};

