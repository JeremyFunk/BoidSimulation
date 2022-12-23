#include "gl_wrapper.h"

namespace Engine {
    namespace Core {
        void GLPrepare() {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
        void GLPostRender(WindowData& window) {
            glfwSwapBuffers(window.window);
            window.PreparePoll();
            glfwPollEvents();
        }
    }
}