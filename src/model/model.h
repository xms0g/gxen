#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "texture.h"
#include "../renderer/shader.h"

class Model {
public:
    Model(std::string const& path, const std::string& vertexPath, const std::string& fragmentPath);

    [[nodiscard]] Shader* shader() const { return mShader.get(); }

    void draw() const;

private:
    void loadModel(std::string const& path);

    void processNode(const aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);

    std::unordered_set<std::string> mTexturesLoaded;
    std::vector<Mesh> mMeshes;
    std::string mDirectory;
    std::unique_ptr<Shader> mShader;
};

