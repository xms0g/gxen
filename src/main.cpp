#include "core/engine.h"

int main() {
    Engine xngn;

    while (xngn.IsRunning()) {
        xngn.ProcessInput();
        xngn.Update();
        xngn.Render();
    }

    return 0;

}
