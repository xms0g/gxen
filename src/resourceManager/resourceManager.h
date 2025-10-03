#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "texture.h"

class Shader;
enum class DebugMode;
class Mesh;

class ResourceManager {
public:
	ResourceManager(const ResourceManager&) = delete;

	ResourceManager& operator=(const ResourceManager&) = delete;

	static ResourceManager& instance();

	void loadModel(size_t entityID, const char* file);

	[[nodiscard]] const std::vector<Mesh>* getMeshes(const size_t entityID) const {
		return &mEntityMeshes.at(entityID);
	}

	[[nodiscard]] const std::vector<Texture>* getTextures(const size_t entityID) const {
		return &mEntityTextures.at(entityID);
	}

private:
	explicit ResourceManager() = default;

	~ResourceManager() = default;

	void processNode(const aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	void loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);

	std::string mDirectory;
	std::vector<Mesh> mMeshes;
	std::vector<Texture> mTextures;
	std::unordered_map<size_t, std::vector<Mesh> > mEntityMeshes;
	std::unordered_map<size_t, std::vector<Texture> > mEntityTextures;
	std::unordered_set<std::string> mTexturesLoaded;
};
