#include "core/engine.h"
#include "core/camera.h"
#include "scene/scene.h"
#include "model/model.h"
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
    const auto scene = std::make_unique<Scene>();

    xngn.init(scene.get());

    std::unique_ptr<Entity> model = std::make_unique<Model>(fs::path(ASSET_DIR + "backpack/backpack.obj"),
                                                            fs::path(SHADER_DIR + "model.vert"),
                                                            fs::path(SHADER_DIR + "model.frag"));
    scene->addEntity(model);

    xngn.run();

    return 0;

}
