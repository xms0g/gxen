#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "texture.h"
#include "../entity/entity.hpp"
#include "../renderer/shader.h"

class Model: public Entity {
public:
    Model(std::string const& path, const std::string& vertexPath, const std::string& fragmentPath);

    [[nodiscard]] Shader* getShader() const override { return mShader.get(); }

    void draw() const override;

private:
    void loadModel(std::string const& path);

    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

    std::unordered_set<std::string> mTexturesLoaded;
    std::vector<Mesh> mMeshes;
    std::string mDirectory;
    std::unique_ptr<Shader> mShader;
};

