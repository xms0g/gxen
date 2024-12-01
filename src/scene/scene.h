#pragma once
#include <vector>

class Model;
class Scene {
public:
    Scene() = default;

    ~Scene() = default;

    void addModel(std::unique_ptr<Model>& model);

    std::vector<std::unique_ptr<Model>>& getModels();

private:
    std::vector<std::unique_ptr<Model>> models;
};
