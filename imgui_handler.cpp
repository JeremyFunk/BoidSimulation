#include "imgui_handler.h"
namespace Engine {
	namespace Core {
		void SetupImGui(GLFWwindow* window) {
			IMGUI_CHECKVERSION();

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			ImGui::StyleColorsDark();
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 330");
			ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;
		}


		void PrepareImGui() {
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}
		void RenderImGui() {
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
	}
}