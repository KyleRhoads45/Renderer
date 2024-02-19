#pragma once
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include "Base.h"

// Matches GLFW Macros
constexpr auto INPUT_RELEASED = 0;
constexpr auto INPUT_PRESSED = 1;
constexpr auto INPUT_HELD = 2;

class Input {
public:
    static void Init(GLFWwindow* window);
    static void Update(GLFWwindow* window);
    static bool OnKeyHold(const i32 key);
    static bool OnKeyPress(const i32 key);
    static bool OnMouseHold(const i32 button);
    static bool OnMousePress(const i32 button);
    static glm::vec2 MousePosition();
    static glm::vec2 GetMouseDeltas();
private:
    static void SwitchPressToHeld();
    static void UpdateMouseDeltas(GLFWwindow* window);
    static void KeyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);
    static void MouseCallback(GLFWwindow* window, i32 button, i32 action, i32 mods);
private: 
    inline static std::unordered_map<i32, i32> s_KeyMap;
    inline static std::unordered_map<i32, i32> s_MouseMap;
    inline static glm::vec2 s_MouseDelta;
    inline static f64 s_MouseXPos = 0.0; 
    inline static f64 s_MouseYPos = 0.0;
};