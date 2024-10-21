#include "namespaces.h"
#include "ctpl_stl.h"
#include <cmath>
#include <raylib.h>
#include <thread>

namespace screen
{
const int WIDTH = 2256;
const int HEIGHT = 1504;
const bool FULL_SCREEN = true;
} // namespace screen

namespace game
{
int SOLDIER_COUNT = 500;
const int TILE_SIZE = 5;
float DELTA;
const int WIDTH = screen::WIDTH / TILE_SIZE;
const int HEIGHT = screen::HEIGHT / TILE_SIZE;
const int GRAPH_RESOLUTION = 35;
const int bbSubdivisionsX = 40;
const int bbSubdivisionsY = 30;
const int numThreads = std::fmax(1u, std::thread::hardware_concurrency());
ctpl::thread_pool pool(numThreads);
} // namespace game

namespace debug
{
bool boundingBoxes = false;
bool graph = false;
bool paths = true;
} // namespace debug

namespace shoshone
{
const Color yellow = {255, 225, 143, 255};
const Color orange = {255, 159, 116, 255};
const Color red = {222, 77, 99, 255};
const Color pink = {124, 24, 60, 255};
const Color maroon = {49, 17, 45, 255};
} // namespace shoshone
