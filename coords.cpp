#include "coords.h"

Coords::Coords(int x, int y) : _x(x), _y(y) {};

bool Coords::operator==(const Coords& coords) const { return _x == coords._x && _y == coords._y; };