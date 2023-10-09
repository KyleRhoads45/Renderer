#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "core/Base.h"
#include "Model.h"

Entity Model::Instantiate(const char* meshPath, Material* mat) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(meshPath, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	assert(scene || !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || scene->mRootNode);

	return ProcessNode(scene, scene->mRootNode, Entity::Null(), mat);
}

Entity Model::ProcessNode(const aiScene* scene, const aiNode* node, Entity parent, Material* mat) {
	auto entity = Registry::Create();
	entity.Add<LocalToWorld>();
	auto& trans = entity.Add<Transform>();

	aiVector3t<f32> scale;
	aiVector3t<f32> position;
	aiQuaterniont<f32> rotation;
	node->mTransformation.Decompose(scale, rotation, position);
	
	trans.position = glm::vec3(position.x, position.y, position.z);
	trans.rotation = glm::quat(rotation.w, rotation.x, rotation.y, rotation.z);
	trans.scale = glm::vec3(scale.x, scale.y, scale.z);

	if (parent != Entity::Null()) {
		entity.Add<Parent>().entity = parent;
	}

	for (u32 i = 0; i < node->mNumMeshes; i++) {
		auto& meshRenderer = entity.Add<MeshRenderer>();
		const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshRenderer.mesh = Mesh::FromAssimpMesh(mesh);
		meshRenderer.material = mat;
	}

	if (node->mNumChildren > 0) {
		entity.Add<Children>();
		for (u32 i = 0; i < node->mNumChildren; i++) {
			auto child = ProcessNode(scene, node->mChildren[i], entity, mat);
			entity.Get<Children>().entities.push_back(child);
		}
	}

	return entity;
}

