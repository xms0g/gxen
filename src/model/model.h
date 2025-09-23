#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "texture.h"

class Shader;
class Model {
public:
    explicit Model(std::string const& path);

    void draw(const Shader* shader) const;

private:
    void loadModel(std::string const& path);

    void processNode(const aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);

    std::unordered_set<std::string> mTexturesLoaded;
    std::vector<Mesh> mMeshes;
    std::string mDirectory;
};

