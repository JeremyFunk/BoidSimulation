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

float deltaTime = 0.0f;
float lastFrame = 0.0f;

using namespace Engine;

int main()
{
    if (SetupWindow() == -1) {
        return -1;
    }

	Core::SetupImGui(GetWindowData().window);
	auto shader = Core::Shader::LoadShaders("vertex.vs", "fragment.fs");
	auto uniforms = Core::Shader::LoadUniforms(shader);
	auto camera = Camera();
	camera.cameraPos = glm::vec3(0.0, 0.0, 15.0);

	std::cout << uniforms.view << std::endl;
	std::cout << uniforms.projection << std::endl;

	Boid boids[BOID_COUNT];

	for (int i = 0; i < BOID_COUNT; i++) {
		auto b = Boid();
		b.randomize(i);
		boids[i] = b;
	}
	auto timer = Timer();
	float t = 0.0f;
	float separation = 1.f; float alignment = 1.f; float cohesion = 0.2f; float desiredSeparation = 2.f; float visionRadius = 2.5f; float angleCutoff = -.2f;
	float minSpeed = 2.f; float maxSpeed = 6.f; float inertia = .2f;
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
					boids[i + boidIndex].run(boids, i + boidIndex, separation, alignment, cohesion, desiredSeparation, visionRadius, angleCutoff, maxSpeed);
				}
			});
			//threadPool.enqueue([&boids, boidIndex, numBoids, separation, alignment, cohesion, desiredSeparation, visionRadius, angleCutoff] {
			boidIndex += numBoids;
		}
		//threadPool.stop();

		//std::cout << timer.elapsedMilliseconds() << std::endl;


		glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)GetWindowData().screenWidth / (float)GetWindowData().screenHeight, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(camera.cameraPos, camera.cameraPos + camera.cameraFront, camera.cameraUp);
		//std::cout << camera.cameraPos.x << std::endl;
		Core::Shader::PrepareShader(uniforms, shader, projection, view);
		Core::Shader::RenderTemp(uniforms, boids);
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