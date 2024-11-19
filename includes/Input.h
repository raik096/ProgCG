#pragma once

#include <GLFW/glfw3.h>
#include "../vendor/imgui/imgui.h"
#include <glm/glm.hpp>

class Input
{
public:

    static bool GetMousePressed(int mouseButton)
    {
        if (mouseButton == 0)
            return leftMouseButtonPressed;
        else
            return rightMouseButtonPressed;
    }

    static bool GetMouseReleased(int mouseButton)
    {
        if (mouseButton == 0)
            return leftMouseButtonReleased;
        else
            return rightMouseButtonReleased;
    }

    static float GetMouseScroll()
    {
        return oldMouseScroll;
    }

    static glm::vec2 MousePosition()
    {
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);
        return glm::vec2((float)x, (float)y);
    }

    static void SetCursorVisibility(bool show)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, (show) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    static void setupMouseInputs(GLFWwindow* window)
    {
        m_Window = window;
        glfwSetMouseButtonCallback(window, Input::mouseCallback);
        glfwSetScrollCallback(window, Input::scrollCallback);
    }

    static void Reset()
    {
        oldMouseScroll = 0;
        leftMouseButtonPressed = false;
        rightMouseButtonPressed = false;

        leftMouseButtonReleased = false;
        leftMouseButtonReleased = false;
    }

private:
    static GLFWwindow* m_Window;
    static bool leftMouseButtonPressed;
    static bool rightMouseButtonPressed;

    static bool leftMouseButtonReleased;
    static bool rightMouseButtonReleased;

    static double oldMouseScroll;
    static float mouseScrollDelta;

    static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        if (button == GLFW_MOUSE_BUTTON_LEFT)
            leftMouseButtonPressed = (action == GLFW_PRESS);

        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            rightMouseButtonPressed = (action == GLFW_PRESS);

        //TODO: Da unire agli if sopra
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            leftMouseButtonReleased = (action == GLFW_RELEASE);

        if (button == GLFW_MOUSE_BUTTON_RIGHT)
            rightMouseButtonReleased = (action == GLFW_RELEASE);
    }

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        if (ImGui::GetIO().WantCaptureMouse)
            return;

        oldMouseScroll = yoffset;
    }
};