#pragma once
#include <vector>
#include <glm/glm.hpp>

struct Particle {
	glm::vec3 pos, speed;
	glm::vec4 color; // Color
	float size, angle, weight;
	float life; // Remaining life of the particle. if < 0 : dead and unused.
	float cameraDistance;
	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameraDistance > that.cameraDistance;
	}
};