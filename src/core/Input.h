#pragma once
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "Base.h"

// Matches GLFW Macros
#define INPUT_RELEASED    0
#define INPUT_PRESSED     1
#define INPUT_HELD        2

class Input {
public:
    static void Init(GLFWwindow* window);
    static void Update(GLFWwindow* window);
    static bool OnKeyHold(i32 key);
    static bool OnKeyPress(i32 key);
    static bool OnMouseHold(i32 button);
    static bool OnMousePress(i32 button);
    static glm::vec2 GetMouseDeltas();
private:
    static void SwitchPressToHeld();
    static void UpdateMouseDeltas(GLFWwindow* window);
    static void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
    static void MouseCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);
    
    inline static std::unordered_map<i32, i32> keyMap;
    inline static std::unordered_map<i32, i32> mouseMap;
    inline static glm::vec2 mouseDelta;
    inline static f64 lastMouseXPos = 0.0; 
    inline static f64 lastMouseYPos = 0.0;
};