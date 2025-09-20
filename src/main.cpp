#include "core/engine.h"
#include "core/camera.h"
#include "scene/scene.h"
#include "model/model.h"
#include "io/filesystem.hpp"
#include "config/config.hpp"
#include "light/light.h"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

#define STRINGIFY0(s) # s
#define STRINGIFY(s) STRINGIFY0(s)
#define VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

int main() {
    XEngine xngn;
    const auto scene = std::make_unique<Scene>();

    xngn.init(scene.get());

    std::unique_ptr<Entity> model = std::make_unique<Model>(fs::path(ASSET_DIR + "backpack/backpack.obj"),
                                                            fs::path(SHADER_DIR + "model.vert"),
                                                            fs::path(SHADER_DIR + "model.frag"));
    model->position = glm::vec3(0.0f);
    model->scale = 1.0f;
    model->rotation = 0.1f;

    scene->addEntity(model);

    std::unique_ptr<Entity> light = std::make_unique<Light>(glm::vec3(3.2f, 1.0f, 2.0f),
                                                            fs::path(SHADER_DIR + "light.vert"),
                                                            fs::path(SHADER_DIR + "light.frag"));
    light->scale = 0.2f;
    scene->addEntity(light);

    xngn.run();

    return 0;
}
