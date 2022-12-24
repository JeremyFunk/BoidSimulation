#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <vector>
#include "boid.h"
#include "constants.h"
#include "particleSystem.h"
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
            struct ParticleShaderUniforms {
                GLuint VP, CameraRight_worldspace, CameraUp_worldspace;
                GLuint VAO;
                GLuint billboardPositionBuffer, billboardColorBuffer, billboardVertexBuffer;
            };
            GLuint LoadUniform(const GLuint program, const char* uniform);
            ShaderUniforms LoadUniforms(const GLuint program);
            ParticleShaderUniforms LoadParticleUniforms(const GLuint program);
            void PrepareParticleShader(const ParticleShaderUniforms u, const GLuint program, const glm::mat4 vp, glm::vec3 camRight, glm::vec3 camUp);
            void PrepareShader(const ShaderUniforms u, const GLuint program, const glm::mat4 projection, const glm::mat4 view);
            GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
            void RenderParticles(const ParticleShaderUniforms u, Particle particles[], GLfloat particlePositionData[], GLubyte particleColorData[], int count);
            void Render(const ShaderUniforms u, Boid v[]);
        }
    }
}