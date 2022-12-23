#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
namespace Engine {
	namespace Core {
		void SetupImGui(GLFWwindow* window);
		void PrepareImGui();
		void RenderImGui();
	}
}