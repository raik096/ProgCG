#include "Input.h"

bool Input::leftMouseButtonPressed = false;
bool Input::rightMouseButtonPressed = false;

bool Input::leftMouseButtonReleased = false;
bool Input::rightMouseButtonReleased = false;

double Input::oldMouseScroll = 0;
float Input::mouseScrollDelta = 0;

GLFWwindow* Input::m_Window = nullptr;