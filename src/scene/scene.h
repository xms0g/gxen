#pragma once
#include <vector>
#include "../model/model.h"

class Scene {
public:
    Scene() = default;

    void addModel(std::unique_ptr<Model>& model);

    std::vector<std::unique_ptr<Model>>& getModels();

private:
    std::vector<std::unique_ptr<Model>> models;
};
