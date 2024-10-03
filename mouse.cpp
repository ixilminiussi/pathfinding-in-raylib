#include "mouse.h"
#include "soldier.h"
#include "namespaces.h"
#include <math.h>

Mouse* Mouse::instance = nullptr;

using namespace shoshone;
Mouse::Mouse() :
	fill({ yellow.r, yellow.g, yellow.b, 100 }),
	outline(yellow),
	mouseStart({ 0.0f, 0.0f }),
	selectionArea({ 0.0f, 0.0f, 0.0f, 0.0f })
{}

Mouse *Mouse::getInstance()  {
	if (instance == nullptr) {
		if (instance == nullptr) {
			instance = new Mouse();
		}
	}
	return instance;
}

void Mouse::update(float dt) {
	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		mouseStart = GetMousePosition();
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
		render();
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		if (IsKeyUp(KEY_LEFT_SHIFT)) {
			deselectAll();
		}
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
		float acceptableDistanceSqr = Soldier::selected.size() * 70.0f;
		for (Soldier* s : Soldier::selected) {
			s->target(GetMousePosition(), acceptableDistanceSqr);
		}
	}
}

void Mouse::render() {
	Vector2 mousePointer = GetMousePosition();
	Vector2 topLeft = {
		std::min(mousePointer.x, mouseStart.x),
		std::min(mousePointer.y, mouseStart.y)
	};
	Vector2 size = {
		std::abs(mousePointer.x - mouseStart.x),
		std::abs(mousePointer.y - mouseStart.y)
	};

	selectionArea = { topLeft.x, topLeft.y, size.x, size.y };
	DrawRectangleRec(selectionArea, fill);
	DrawRectangleLinesEx(selectionArea, 5.0f, outline);
}

void Mouse::deselectAll() const {
	for (Soldier* s : Soldier::army) {
		s->deSelect();
	}
}

const Rectangle& Mouse::getSelectionArea() const {
	return selectionArea;
}
