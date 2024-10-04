#include "namespaces.h"
#include <raylib.h>

namespace screen
{
const int WIDTH = 1080;
const int HEIGHT = 720;
const bool FULL_SCREEN = false;
} // namespace screen

namespace game
{
int SOLDIER_COUNT = 200;
const int TILE_SIZE = 5;
float DELTA;
const int WIDTH = screen::WIDTH / TILE_SIZE;
const int HEIGHT = screen::HEIGHT / TILE_SIZE;
} // namespace game

namespace shoshone
{
const Color yellow = {255, 225, 143, 255};
const Color orange = {255, 159, 116, 255};
const Color red = {222, 77, 99, 255};
const Color pink = {124, 24, 60, 255};
const Color maroon = {49, 17, 45, 255};
} // namespace shoshone