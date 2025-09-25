#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "texture.h"

class Mesh;
class ResourceManager {
public:
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

    static ResourceManager& instance();

	void loadModel(int entityID, std::string const& path);

	std::vector<Mesh>* getMeshes(const int entityID) { return &mEntityMeshes.at(entityID); }

	std::vector<Texture>* getTextures(const int entityID) { return &mEntityTextures.at(entityID); }

private:
	explicit ResourceManager() = default;

	~ResourceManager() = default;

	void processNode(const aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	void loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);

	std::string mDirectory;
	std::vector<Mesh> mMeshes;
	std::vector<Texture> mTextures;
	std::unordered_map<int, std::vector<Mesh>> mEntityMeshes;
	std::unordered_map<int, std::vector<Texture>> mEntityTextures;
	std::unordered_set<std::string> mTexturesLoaded;
};

