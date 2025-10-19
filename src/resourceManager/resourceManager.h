#pragma once
#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct Texture;
using TextureMap = std::unordered_map<uint32_t, std::vector<Texture> >;

class Shader;
class Mesh;

class ResourceManager {
public:
	ResourceManager(const ResourceManager&) = delete;

	ResourceManager& operator=(const ResourceManager&) = delete;

	static ResourceManager& instance();

	[[nodiscard]] const std::vector<Mesh>* getMeshes(size_t entityID) const;

	[[nodiscard]] const TextureMap* getTextures(size_t entityID) const;

	void loadModel(size_t entityID, const char* file);

private:
	explicit ResourceManager() = default;

	~ResourceManager() = default;

	void processNode(const aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	void loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName,
	                          uint32_t materialID);

	std::string mDirectory;
	std::vector<Mesh> mMeshes;
	TextureMap mTexturesFromMesh;
	std::unordered_map<size_t, TextureMap> mTexturesFromEntity;
	std::unordered_map<size_t, std::vector<Mesh> > mMeshesFromEntity;
	std::unordered_set<std::string> mTexturesLoaded;
};
