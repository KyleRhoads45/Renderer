#pragma once
#include <glm/glm.hpp>

struct GuiRect {
    glm::vec2 position;
    glm::vec2 size;
    
    bool ContainsPoint(const glm::vec2& point) const;
    glm::vec2 RelativeCoordinates(const glm::vec2& point) const;
};

class GuiUtils {
public:
    static GuiRect CurrentWindow();
    static glm::vec2 CurrentWindowPosition();
    static glm::vec2 CurrentWindowSize();
    static glm::vec2 MousePosition();
};
