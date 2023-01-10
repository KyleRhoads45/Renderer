#pragma once
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <unordered_map>

//Matches GLFW Macros
#define INPUT_RELEASED    0
#define INPUT_PRESSED     1
#define INPUT_HELD        2

class Input {
public:
    static void Init(GLFWwindow* window);
    static void Update(GLFWwindow* window);
    static bool OnKeyHold(int key);
    static bool OnKeyPress(int key);
    static bool OnMouseHold(int button);
    static bool OnMousePress(int button);
    static glm::vec2 GetMouseDeltas();
private:
    static void SwitchPressToHeld();
    static void UpdateMouseDeltas(GLFWwindow* window);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseCallback(GLFWwindow* window, int button, int action, int mods);
    
    static std::unordered_map<int, int> keyMap;
    static std::unordered_map<int, int> mouseMap;
    static glm::vec2 mouseDelta;
    static double lastMouseXPos; 
    static double lastMouseYPos;
};