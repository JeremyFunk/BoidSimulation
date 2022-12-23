#pragma once
#include <glm/fwd.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include "window_data.h"
struct Camera {
    float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch = 0.0f;
    float fov = 45.0f;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    void move(WindowData& data, float deltaTime) {
        float sensitivity = 0.1;

        yaw -= data.mouseData.xOffset * sensitivity;
        pitch += data.mouseData.yOffset * sensitivity;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);

        float cameraSpeed = static_cast<float>(2.5 * deltaTime);
        if (glfwGetKey(data.window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * cameraFront;
        if (glfwGetKey(data.window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * cameraFront;
        if (glfwGetKey(data.window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(data.window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(data.window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraPos.y += cameraSpeed;
        if (glfwGetKey(data.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cameraPos.y -= cameraSpeed;
    }
};