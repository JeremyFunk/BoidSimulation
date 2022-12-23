#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "window_data.h"

namespace Engine {
    namespace Core {
        void GLPrepare();
        void GLPostRender(WindowData& window);
    }
}