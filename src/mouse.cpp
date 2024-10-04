#include "mouse.h"
#include "soldier.h"
#include "namespaces.h"
#include "world.h"
#include "tile.h"
#include <math.h>
#include <algorithm>

Mouse* Mouse::instance = nullptr;

using namespace shoshone;
Mouse::Mouse() :
	fill({ yellow.r, yellow.g, yellow.b, 100 }),
	outline(yellow),
	mouseStart({ 0.0f, 0.0f }),
	selectionArea({ 0.0f, 0.0f, 0.0f, 0.0f }),
	mode(Mode::Editing),
	editingRadius(50.0f)
{}

Mouse *Mouse::getInstance()  {
	if (instance == nullptr) {
		instance = new Mouse();
	}
	return instance;
}

void Mouse::update(float dt) {
	World* world = World::getInstance();

	switch (mode) {
	case Mode::Editing:
		if (IsKeyPressed(KEY_SPACE)) {
			mode = Mode::Moving;
		}

		editingRadius -= GetMouseWheelMove() * 10.0f;
		if (editingRadius < 20.0f) editingRadius = 20.0f;
		if (editingRadius > 150.0f) editingRadius = 150.0f;

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			world->paintArea(GetMousePosition(), editingRadius, Type::WALL);
		}
		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
			world->paintArea(GetMousePosition(), editingRadius, Type::FLOOR);
		}

		break;
	case Mode::Moving:
		if (IsKeyPressed(KEY_SPACE)) {
			mode = Mode::Editing;
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			mouseStart = GetMousePosition();
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
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
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			if (IsKeyUp(KEY_LEFT_SHIFT)) {
				deselectAll();
			}
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
			float acceptableDistanceSqr = Soldier::selected.size() * 60.0f;
			for (Soldier* s : Soldier::selected) {
				s->target(GetMousePosition(), acceptableDistanceSqr);
			}
		}
		break;
	}
}

void Mouse::render() {
	switch (mode) {
	case Mode::Editing:
		DrawCircleV(GetMousePosition(), editingRadius, {
			shoshone::yellow.r,
			shoshone::yellow.g,
			shoshone::yellow.b,
			100
			});
		break;
	case Mode::Moving:
		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
			DrawRectangleRec(selectionArea, fill);
			DrawRectangleLinesEx(selectionArea, 3.0f, outline);
		}
		break;
	}
}

void Mouse::deselectAll() const {
	for (Soldier* s : Soldier::army) {
		s->deselect();
	}
}

const Rectangle& Mouse::getSelectionArea() const {
	return selectionArea;
}
