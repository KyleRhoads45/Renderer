#include <imgui/imgui.h>
#include "GuiUtils.h"

bool GuiRect::ContainsPoint(const glm::vec2& point) const {
	const glm::vec2 minRectPos(position.x, position.y);
	const glm::vec2 maxRectPos(position.x + size.x, position.y + size.y);
	
	bool pointInsideWidth = point.x >= minRectPos.x && point.x <= maxRectPos.x;
	bool pointInsideHeight = point.y >= minRectPos.y && point.y <= maxRectPos.y;
	return pointInsideWidth && pointInsideHeight;
}

glm::vec2 GuiRect::RelativeCoordinates(const glm::vec2& point) const {
	return { point.x - position.x, size.y - (point.y - position.y) };
}

GuiRect GuiUtils::CurrentWindow() {
	return { CurrentWindowPosition(), CurrentWindowSize() };	
}

glm::vec2 GuiUtils::CurrentWindowPosition() {
	ImVec2 screenPos = ImGui::GetCursorScreenPos();
	return { screenPos.x, screenPos.y };
}

glm::vec2 GuiUtils::CurrentWindowSize() {
	ImVec2 windowSize = ImGui::GetWindowSize();
	return { windowSize.x, windowSize.y };
}

glm::vec2 GuiUtils::MousePosition() {
	ImVec2 mousePos = ImGui::GetMousePos();
	return { mousePos.x, mousePos.y };	
}
