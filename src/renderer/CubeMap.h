#pragma once
#include <array>
#include <cstdint>
#include <string>

class CubeMap {

public:

	CubeMap() = default;
	CubeMap(const std::array<std::string, 6>& images);
	void Bind();

private:

	uint32_t id;

};

