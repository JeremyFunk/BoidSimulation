#include "window.h"
#include <cstdio>


WindowData windowData;



void windowSizeCallback(GLFWwindow* window, int width, int height)
{
	windowData.screenWidth = width;
	windowData.screenHeight = height;
	glViewport(0, 0, windowData.screenWidth, windowData.screenHeight);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	// Mouse did not get updated yet
	if (windowData.mouseData.lastX == -4206969)
	{
		windowData.mouseData.lastX = xpos;
		windowData.mouseData.lastY = ypos;
	}

	windowData.mouseData.xOffset = windowData.mouseData.lastX - xpos;
	windowData.mouseData.yOffset = windowData.mouseData.lastY - ypos; // reversed since y-coordinates go from bottom to top

	windowData.mouseData.xPos = xpos;
	windowData.mouseData.yPos = ypos;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	windowData.keyStates[key] = action;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	windowData.mouseData.scrollSpeed = yoffset;
}

int SetupWindow() {

	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 1);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glEnable(GL_DEPTH_TEST);

	// Open a window and create its OpenGL context
	windowData.window = glfwCreateWindow(windowData.screenWidth, windowData.screenHeight, "Ray Marching Editor", NULL, NULL);
	if (windowData.window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(windowData.window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(windowData.window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(windowData.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Dark blue background
	/*glClearColor(0.1f, 0.1f, 0.2f, 0.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	glfwSetWindowSizeCallback(windowData.window, windowSizeCallback);
	glfwSetCursorPosCallback(windowData.window, mouseCallback);
	glfwSetKeyCallback(windowData.window, keyCallback);
	glfwSetScrollCallback(windowData.window, scrollCallback);
	return 0;
}


int CleanUp() {
	return 0;
}

WindowData& GetWindowData() {
	return windowData;
}