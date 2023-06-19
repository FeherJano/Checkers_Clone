//
// Created by feher on 7/20/2022.
//

#ifndef SKIRMISH_GAME_COORD_H
#define SKIRMISH_GAME_COORD_H

struct Coord {

    int x;
    int y;

    Coord(int new_x = 0, int new_y = 0) : x(new_x), y(new_y) {}
    ~Coord() {}

    Coord* Average(Coord* coord) {
        return new Coord((x + coord->x) >> 1, (y + coord->y) >> 1);
    }

    bool Equal(Coord* coord) {
        return x == coord->x && y == coord->y;
    }

};

#endif //SKIRMISH_GAME_COORD_H
