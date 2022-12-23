#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    float life;
    float size;
};