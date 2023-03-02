#pragma once
#include <memory>
#include <glm/glm.hpp>
#include "ecs/Entity.h"
#include "Mesh.h"
#include "Material.h"

Entity InstantiateModel(const char* meshPath, Material* mat);
Mesh LoadMesh(const char* meshPath);