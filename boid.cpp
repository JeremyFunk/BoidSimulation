#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include "Boid.h"

const float bounds = 10.0;

Boid::Boid() {
    predator = false;
    location = glm::vec3(0.0);
    velocity = glm::vec3(0.0);
    acceleration = glm::vec3(0.0);
    particleTimer = 0.1f;
    for (int i = 0; i < BOID_COUNT; i++) {
        distances[i] = 0.f;
    }
}

float randomNum(float low, float high) {
    return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
}

void Boid::randomize(int i) {
    location = glm::vec3(randomNum(-bounds, bounds), randomNum(-bounds, bounds), randomNum(-bounds, bounds));
    velocity = glm::vec3(randomNum(-5.0, 5.0), randomNum(-5.0, 5.0), randomNum(-5.0, 5.0));

    /*if (i < 5) {
        predator = true;
    }*/
    //velocity = glm::vec3(0.0);
}

double inline __declspec (naked) __fastcall sqrtFast(double n)
{
    _asm fld qword ptr[esp + 4]
        _asm fsqrt
    _asm ret 8
}

void Boid::run(Boid v[], int self, float separation, float alignment, float cohesion, float desiredSeparation, float visionRadius, float angleCutoff, float maxSpeed, float dt) {
    float visionRadius2 = visionRadius * visionRadius;
    glm::vec3 repulsion = glm::vec3(0.0);
    glm::vec3 directions = glm::vec3(0.0);
    glm::vec3 center = glm::vec3(0.0);
    glm::vec3 centerAttraction = glm::vec3(0.0);
    float l = glm::length(location);
    if (l > bounds - 2.) {
        float centerAttractionForce = (l - (bounds - 2.)) * .2f;
        centerAttraction = (-location) * centerAttractionForce * centerAttractionForce;
    }
    else {
        centerAttraction = (-location) * 0.1f;
    }

    int nearby = 0;
    int nearbySep = 0;
    glm::vec3 velocityN = glm::normalize(velocity);
    for (int i = 0; i < BOID_COUNT; i++) {
        distances[i] -= maxSpeed; 
        //int num = *reinterpret_cast<int*>(&distances[i]);
        if (distances[i] < visionRadius2 && i != self) {
            //glm::vec3 connect = v[i].location - location;
            float connectX = v[i].location.x - location.x;
            float connectY = v[i].location.y - location.y;
            float connectZ = v[i].location.z - location.z;
            float lengthSquared = connectX * connectX + connectY * connectY + connectZ * connectZ;
            distances[i] = lengthSquared;
            if (lengthSquared <= visionRadius2) {
                float length = sqrtFast(lengthSquared);
                float invLength = 1.0 / length;
                glm::vec3 connectN = glm::vec3(connectX * invLength, connectY * invLength, connectZ * invLength);

                //float angle = glm::dot(velocityN, connectN);
                float angle = velocityN.x * connectN.x + velocityN.y * connectN.y + velocityN.z + connectN.z;

                if (angleCutoff > angle) {
                    center += v[i].velocity;
                    if (length <= desiredSeparation) {
                        if (predator && !v[i].predator){
                            repulsion += connectN;
                        }
                        else if(!predator && !v[i].predator){
                            repulsion -= connectN;
                        }
                        else if (!predator && v[i].predator) {
                            repulsion -= connectN * 10.f;
                        }
                        repulsion -= connectN;
                        nearbySep++;
                    }
                    directions.x += connectX;
                    directions.y += connectY;
                    directions.z += connectZ;
                    nearby++;
                }
            }
        }
    }
    center /= float(nearby);
    if (nearby > 0) {
        if (nearbySep > 0)
        {
            repulsion = (repulsion / float(nearbySep)) * separation;
        }
        acceleration = repulsion + (directions / float(nearby)) * alignment + (center - velocity) * cohesion;
    }

    acceleration += centerAttraction;
}

void Boid::update(float delta, float minSpeed, float maxSpeed, float inertia) {
    velocity += acceleration * .2f * inertia;
    if (abs(glm::length(velocity)) > maxSpeed) {
        velocity = glm::normalize(velocity) * maxSpeed;
    }
    if (abs(glm::length(acceleration)) > 5.f) {
        acceleration = glm::normalize(acceleration) * 5.f;
    }
    else if (abs(glm::length(velocity)) < minSpeed) {
        velocity = glm::normalize(velocity) * minSpeed;
    }
    location += velocity * delta;
    /*if (glm::length(location) > bounds * 2) {
        location = -glm::normalize(location) * bounds * .5f;
    }*/
}