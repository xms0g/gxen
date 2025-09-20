#include "core/engine.h"
#include "scene/scene.h"
#include "model/model.h"
#include "light/light.h"
#include "io/filesystem.hpp"
#include "config/config.hpp"

#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 0

#define STRINGIFY0(s) # s
#define STRINGIFY(s) STRINGIFY0(s)
#define VERSION STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)

int main() {
    XEngine xngn;
    xngn.init();

    auto scene = std::make_unique<Scene>();
    std::unique_ptr<Entity> light = std::make_unique<Light>(glm::vec3(1.2f, 1.0f, 2.0f),
                                                            fs::path(SHADER_DIR + "light.vert"),
                                                            fs::path(SHADER_DIR + "light.frag"));

    std::unique_ptr<Entity> model = std::make_unique<Model>(fs::path(ASSET_DIR + "backpack/backpack.obj"),
                                                            fs::path(SHADER_DIR + "model.vert"),
                                                            fs::path(SHADER_DIR + "model.frag"));
    scene->addEntity(model);
    //scene->addEntity(light);

    xngn.addScene(scene);

    xngn.run();

    return 0;

}
