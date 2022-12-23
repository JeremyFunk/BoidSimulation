#pragma once
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <glm/glm.hpp>
#include "constants.h"
#include <array>

class Boid {
public:
    bool predator;
    glm::vec3 location;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float* distances = new float[BOID_COUNT];
    Boid();

    void run(Boid v[], int self, float separation, float alignment, float cohesion, float desiredSeparation, float visionRadius, float angleCutoff, float maxSpeed);
    void randomize(int i);
    void update(float delta, float minSpeed, float maxSpeed, float inertia);
};