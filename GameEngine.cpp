// For Sleep
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "gl_wrapper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "window.h"
#include <iostream>
#include "imgui_handler.h"
#include "shader.h"
#include "camera.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "boid.h"
#include "timer.h"
#include <thread>
#include "threadPool.h"
#include "../octree/octree.h"
#include "particleSystem.h"

float deltaTime = 0.0f;
float lastFrame = 0.0f;

using namespace Engine;

Particle particles[MAX_PARTICLES];
int lastUsedParticle = 0;

int FindUnusedParticle() {

	for (int i = lastUsedParticle; i < MAX_PARTICLES; i++) {
		if (particles[i].life < 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++) {
		if (particles[i].life < 0) {
			lastUsedParticle = i;
			return i;
		}
	}

	return 0; // All particles are taken, override the first one
}

void SortParticles() {
	std::sort(&particles[0], &particles[MAX_PARTICLES]);
}

int main()
{
    if (SetupWindow() == -1) {
        return -1;
    }

	Core::SetupImGui(GetWindowData().window);
	auto shader = Core::Shader::LoadShaders("vertex.vs", "fragment.fs");
	auto particleShader = Core::Shader::LoadShaders("particle.vs", "particle.fs");
	auto uniforms = Core::Shader::LoadUniforms(shader);
	auto particleUniforms = Core::Shader::LoadParticleUniforms(particleShader);
	auto camera = Camera();
	camera.cameraPos = glm::vec3(0.0, 0.0, 15.0);

	std::cout << uniforms.view << std::endl;
	std::cout << uniforms.projection << std::endl;

	Boid* boids = new Boid[BOID_COUNT];
	static GLfloat* g_particule_position_size_data = new GLfloat[MAX_PARTICLES * 4];
	static GLubyte* g_particule_color_data = new GLubyte[MAX_PARTICLES * 4];

	for (int i = 0; i < MAX_PARTICLES; i++) {
		particles[i].life = -1.0f;
		particles[i].cameraDistance = -1.0f;
	}

	for (int i = 0; i < BOID_COUNT; i++) {
		auto b = Boid();
		b.randomize(i);
		boids[i] = b;
	}
	auto timer = Timer();
	float t = 0.0f;
	float separation = 1.f; float alignment = 1.f; float cohesion = 0.2f; float desiredSeparation = 2.f; float visionRadius = 2.5f; float angleCutoff = -.2f;
	float minSpeed = 2.f; float maxSpeed = 6.f; float inertia = .2f;
	float particlesPerSecond = 200.f, particleLifetime = .1f, particleStartSize = .1f;
	bool cameraInput = true;
	std::thread threads[NUM_THREADS];
	bool first = true;
	do {
		timer.start();
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		t += deltaTime;

		WindowData& windowData = GetWindowData();

		float particlesEvery = 1.f / particlesPerSecond;
		float particleLifetimeInv = 1.f / particleLifetime;
		for (int i = 0; i < BOID_COUNT; i++) {
			boids[i].particleTimer -= deltaTime;
			int totalRuns = 0;
			while(boids[i].particleTimer < 0.f && totalRuns < 4) {
				totalRuns++;
				float lifetimeInterval = 1.f + particleLifetime * boids[i].particleTimer;
				boids[i].particleTimer += particlesEvery;
				int particleIndex = FindUnusedParticle();
				particles[particleIndex].life = particleLifetime * lifetimeInterval; // This particle will live 5 seconds.
				particles[particleIndex].pos = boids[i].location + boids[i].velocity * boids[i].particleTimer;
				float spread = 0.f;
				glm::vec3 randomdir = glm::vec3(
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f,
					(rand() % 2000 - 1000.0f) / 1000.0f
				);
				//particles[particleIndex].speed = ((boids[i].velocity) + randomdir * spread) * .1f;
				particles[particleIndex].speed = glm::vec3(0.f);
				particles[particleIndex].color = glm::vec4(200 * lifetimeInterval, 200 * lifetimeInterval, 200 * lifetimeInterval, 127 * lifetimeInterval);

				//particles[particleIndex].size = (rand() % 1000) / 20000.0f + 0.01f;
				particles[particleIndex].size = (particleStartSize * lifetimeInterval);
			}
		}

		//int newparticles = (int)(deltaTime * BOID_COUNT * 10);
		//if (newparticles > (int)(0.016f * 10000.0))
		//	newparticles = (int)(0.016f * 10000.0);
		//for (int i = 0; i < newparticles; i++) {
		//	int particleIndex = FindUnusedParticle();
		//	particles[particleIndex].life = 5.0f; // This particle will live 5 seconds.
		//	particles[particleIndex].pos = glm::vec3(0, 0, -20.0f);

		//	float spread = 1.5f;
		//	glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
		//	// Very bad way to generate a random direction; 
		//	// See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
		//	// combined with some user-controlled parameters (main direction, spread, etc)
		//	glm::vec3 randomdir = glm::vec3(
		//		(rand() % 2000 - 1000.0f) / 1000.0f,
		//		(rand() % 2000 - 1000.0f) / 1000.0f,
		//		(rand() % 2000 - 1000.0f) / 1000.0f
		//	);

		//	particles[particleIndex].speed = maindir + randomdir * spread;


		//	// Very bad way to generate a random color
		//	particles[particleIndex].r = rand() % 256;
		//	particles[particleIndex].g = rand() % 256;
		//	particles[particleIndex].b = rand() % 256;
		//	particles[particleIndex].a = (rand() % 256) / 3;

		//	particles[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;

		//}

		int ParticlesCount = 0;
		for (int i = 0; i < MAX_PARTICLES; i++) {

			Particle& p = particles[i]; // shortcut

			if (p.life > 0.0f) {

				// Decrease life
				p.life -= deltaTime;
				if (p.life > 0.0f) {
					float lifeAbs = p.life * particleLifetimeInv;
					// Simulate simple physics : gravity only, no collisions
					//p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)deltaTime * 0.5f;
					float multiplier = deltaTime * particleLifetimeInv;
					p.pos += p.speed * deltaTime;
					float px = p.pos.x - camera.cameraPos.x;
					float py = p.pos.y - camera.cameraPos.y;
					float pz = p.pos.z - camera.cameraPos.z;
					p.cameraDistance = px * px + py * py + pz * pz;
					//p.size -= (multiplier * particleStartSize * .5f);
					/*p.color.x -= multiplier * 255;
					p.color.y -= multiplier * 255;
					p.color.z -= multiplier * 255;*/
					//p.color.w -= multiplier * 255 * particleLifetime;
					if (p.color.w <= 0.f) {
						p.color.w = 0.f;
					}
					

					// Fill the GPU buffer
					g_particule_position_size_data[4 * ParticlesCount + 0] = p.pos.x;
					g_particule_position_size_data[4 * ParticlesCount + 1] = p.pos.y;
					g_particule_position_size_data[4 * ParticlesCount + 2] = p.pos.z;

					g_particule_position_size_data[4 * ParticlesCount + 3] = p.size * lifeAbs;

					g_particule_color_data[4 * ParticlesCount + 0] = (char)p.color.x;
					g_particule_color_data[4 * ParticlesCount + 1] = (char)p.color.y;
					g_particule_color_data[4 * ParticlesCount + 2] = (char)p.color.z;
					g_particule_color_data[4 * ParticlesCount + 3] = (char)p.color.w * std::min(lifeAbs * 2.f, 1.0f);
				}
				else {
					g_particule_position_size_data[4 * ParticlesCount + 0] = 0.f;
					g_particule_position_size_data[4 * ParticlesCount + 1] = 0.f;
					g_particule_position_size_data[4 * ParticlesCount + 2] = 0.f;

					g_particule_position_size_data[4 * ParticlesCount + 3] = 0.f;

					g_particule_color_data[4 * ParticlesCount + 0] = 0;
					g_particule_color_data[4 * ParticlesCount + 1] = 0;
					g_particule_color_data[4 * ParticlesCount + 2] = 0;
					g_particule_color_data[4 * ParticlesCount + 3] = 0;
					// Particles that just died will be put at the end of the buffer in SortParticles();
					p.cameraDistance = -1.0f;
				}

				ParticlesCount++;

			}
		}
		SortParticles();

		if(cameraInput)
			camera.move(windowData, deltaTime);
		
		Core::GLPrepare();
		if (!first) {
			for (int i = 0; i < NUM_THREADS; i++) {
				threads[i].join();
			}
			for (int i = 0; i < BOID_COUNT; i++) {
				boids[i].update(deltaTime, minSpeed, maxSpeed, inertia);
			}
		}

		int boidsPerThread = BOID_COUNT / NUM_THREADS;
		int remainingBoids = BOID_COUNT % NUM_THREADS;
		int boidIndex = 0;
		for (int i = 0; i < NUM_THREADS; i++) {
			int numBoids = boidsPerThread;
			if (remainingBoids > 0) {
				numBoids++;
				remainingBoids--;
			}
			threads[i] = std::thread([&boids, boidIndex, numBoids, separation, alignment, cohesion, desiredSeparation, visionRadius, angleCutoff, maxSpeed] {
				for (int i = 0; i < numBoids; i++) {
					boids[i + boidIndex].run(boids, i + boidIndex, separation, alignment, cohesion, desiredSeparation, visionRadius, angleCutoff, maxSpeed * deltaTime, deltaTime);
				}
			});
			//threadPool.enqueue([&boids, boidIndex, numBoids, separation, alignment, cohesion, desiredSeparation, visionRadius, angleCutoff] {
			boidIndex += numBoids;
		}
		//threadPool.stop();


		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)GetWindowData().screenWidth / (float)GetWindowData().screenHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
		//std::cout << camera.cameraPos.x << std::endl;

		Core::Shader::PrepareParticleShader(particleUniforms, particleShader, projection * view, glm::vec3(view[0][0], view[1][0], view[2][0]), glm::vec3(view[0][1], view[1][1], view[2][1]));
		Core::Shader::RenderParticles(particleUniforms, particles, g_particule_position_size_data, g_particule_color_data, ParticlesCount);
		Core::Shader::PrepareShader(uniforms, shader, projection, view);
		Core::Shader::Render(uniforms, boids);


		Core::PrepareImGui();

		ImGui::BeginTabBar("#sdsd");
		ImGui::SliderFloat("Separation", &separation, -1.f, 1.f);
		ImGui::SliderFloat("Alignment", &alignment, -1.f, 1.f);
		ImGui::SliderFloat("Cohesion", &cohesion, -1.f, 1.f);
		ImGui::SliderFloat("Desired Separation", &desiredSeparation, 0.f, 10.f);
		ImGui::SliderFloat("Vision Radius", &visionRadius, 0.f, 20.f);
		ImGui::SliderFloat("Angle cutoff", &angleCutoff, -1.f, 1.f);
		ImGui::SliderFloat("Minimum Speed", &minSpeed, 0.f, 10.f);
		ImGui::SliderFloat("Maximum Speed", &maxSpeed, 0.f, 10.f);
		ImGui::SliderFloat("Inertia", &inertia, 0.f, 2.f);
		ImGui::EndTabBar();

		Core::RenderImGui();
		timer.stop();
		std::cout << timer.elapsedMilliseconds() << std::endl;
		timer.start();
		Core::GLPostRender(windowData);
		//glFinish();
		timer.stop();
		std::cout << timer.elapsedMilliseconds() << std::endl;
		std::cout << "---" << std::endl;

		if (glfwGetKey(windowData.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(windowData.window, true);
		if (windowData.keyStates[GLFW_KEY_TAB] == GLFW_RELEASE) {
			if (cameraInput) {
				glfwSetInputMode(windowData.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				cameraInput = false;
			}
			else {
				glfwSetInputMode(windowData.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				cameraInput = true;
			}
				
		}
		//glFinish();
		//Sleep(1);
		first = false;
	} while (glfwWindowShouldClose(GetWindowData().window) == 0);

	for (int i = 0; i < NUM_THREADS; i++) {
		threads[i].join();
	}
}