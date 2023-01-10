#include "Input.h"

std::unordered_map<int, int> Input::keyMap;
std::unordered_map<int, int> Input::mouseMap;
glm::vec2 Input::mouseDelta;
double Input::lastMouseXPos = 0; 
double Input::lastMouseYPos = 0;

void Input::Init(GLFWwindow* window) {
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    glfwGetCursorPos(window, &lastMouseXPos, &lastMouseYPos);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseCallback);
}

void Input::Update(GLFWwindow* window) {
    UpdateMouseDeltas(window);
    SwitchPressToHeld();
    glfwPollEvents();  
}

bool Input::OnKeyHold(const int key) {
    return (keyMap[key] == INPUT_PRESSED || keyMap[key] == INPUT_HELD);
}

bool Input::OnKeyPress(const int key) {
    return (keyMap[key] == INPUT_PRESSED);
}

bool Input::OnMouseHold(const int button) {
    return (mouseMap[button] == INPUT_PRESSED || mouseMap[button] == INPUT_HELD);
}

bool Input::OnMousePress(const int button) {
    return (mouseMap[button] == INPUT_PRESSED);
}

glm::vec2 Input::GetMouseDeltas() {
    return mouseDelta; 
}

void Input::SwitchPressToHeld() {
    for (auto& key : keyMap) {
        if (key.second == INPUT_PRESSED) {
            key.second = INPUT_HELD; 
        }
    }
    for (auto& key : mouseMap) {
        if (key.second == INPUT_PRESSED) {
            key.second = INPUT_HELD; 
        }
    }
}

void Input::UpdateMouseDeltas(GLFWwindow* window) {
    double curMouseXPos, curMouseYPos;
    glfwGetCursorPos(window, &curMouseXPos, &curMouseYPos);
    
    mouseDelta.x = curMouseXPos - lastMouseXPos;
    mouseDelta.y = curMouseYPos - lastMouseYPos;
    
    lastMouseXPos = curMouseXPos;
    lastMouseYPos = curMouseYPos;
}

void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == INPUT_PRESSED) {
        keyMap[key] = INPUT_PRESSED;
    }
    if (action == INPUT_RELEASED) {
        keyMap[key] = INPUT_RELEASED;
    }
}

void Input::MouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == INPUT_PRESSED) {
        mouseMap[button] = INPUT_PRESSED;
    }
    if (action == INPUT_RELEASED) {
        mouseMap[button] = INPUT_RELEASED;
    }
}
