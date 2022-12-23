#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "boid.h"
#include "constants.h"
namespace Engine {
    namespace Core {
        namespace Shader {
            struct ShaderUniforms {
                GLuint view, projection;
                GLuint position[BOID_COUNT];
                GLuint predators[BOID_COUNT];
                GLuint texbuffer;
                GLuint VAO;
                GLuint VBO;
                GLuint EBO;
            };
            GLuint LoadUniform(const GLuint program, const char* uniform);
            ShaderUniforms LoadUniforms(const GLuint program);
            void PrepareShader(const ShaderUniforms u, const GLuint program, const glm::mat4 projection, const glm::mat4 view);
            GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
            void RenderTemp(const ShaderUniforms u, Boid v[]);
        }
    }
}