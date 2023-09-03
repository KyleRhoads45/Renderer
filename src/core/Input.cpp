#include "Input.h"

void Input::Init(GLFWwindow* window) {
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }
    glfwGetCursorPos(window, &s_LastMouseXPos, &s_LastMouseYPos);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseCallback);
}

void Input::Update(GLFWwindow* window) {
    UpdateMouseDeltas(window);
    SwitchPressToHeld();
    glfwPollEvents(); 
}

bool Input::OnKeyHold(const i32 key) {
    return (s_KeyMap[key] == INPUT_PRESSED || s_KeyMap[key] == INPUT_HELD);
}

bool Input::OnKeyPress(const i32 key) {
    return (s_KeyMap[key] == INPUT_PRESSED);
}

bool Input::OnMouseHold(const i32 button) {
    return (s_MouseMap[button] == INPUT_PRESSED || s_MouseMap[button] == INPUT_HELD);
}

bool Input::OnMousePress(const i32 button) {
    return (s_MouseMap[button] == INPUT_PRESSED);
}

glm::vec2 Input::GetMouseDeltas() {
    return s_MouseDelta; 
}

void Input::SwitchPressToHeld() {
    for (auto& key : s_KeyMap) {
        if (key.second == INPUT_PRESSED) {
            key.second = INPUT_HELD; 
        }
    }
    for (auto& key : s_MouseMap) {
        if (key.second == INPUT_PRESSED) {
            key.second = INPUT_HELD; 
        }
    }
}

void Input::UpdateMouseDeltas(GLFWwindow* window) {
    f64 curMouseXPos, curMouseYPos;
    glfwGetCursorPos(window, &curMouseXPos, &curMouseYPos);
    
    s_MouseDelta.x = curMouseXPos - s_LastMouseXPos;
    s_MouseDelta.y = curMouseYPos - s_LastMouseYPos;
    
    s_LastMouseXPos = curMouseXPos;
    s_LastMouseYPos = curMouseYPos;
}

void Input::KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
    if (action == INPUT_PRESSED) {
        s_KeyMap[key] = INPUT_PRESSED;
    }
    if (action == INPUT_RELEASED) {
        s_KeyMap[key] = INPUT_RELEASED;
    }
}

void Input::MouseCallback(GLFWwindow* window, i32 button, i32 action, i32 mods) {
    if (action == INPUT_PRESSED) {
        s_MouseMap[button] = INPUT_PRESSED;
    }
    if (action == INPUT_RELEASED) {
        s_MouseMap[button] = INPUT_RELEASED;
    }
}
