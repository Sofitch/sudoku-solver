#ifndef COORDS_H
#define COORDS_H

struct Coords {
    int _x;
    int _y;
    Coords(int x, int y);
    bool operator==(const Coords& coords) const;
};

#endif