//
// Created by feher on 7/20/2022.
//

#ifndef SKIRMISH_GAME_TURN_H
#define SKIRMISH_GAME_TURN_H

#include <vector>

#include "state.h"
#include "coord.h"

class State;


class Turn {
public:
    const static int dataLength = 30;

    Turn() {}
    Turn(const std::vector<Coord*> coords, const bool capture)
            : coords_(coords), capture_(capture) {
        Encode();
    }
    ~Turn() {}

    void Decode();

    bool CheckMatching(Turn* turn) const;

    char* data() { return data_; }
    std::vector<Coord*> coords() const { return coords_; }
    bool capture() const { return capture_; }

private:
    const char xMin = 'a';     // min x coordinate
    const char yMin = '1';     // min y coordinate

    void Encode();

    char data_[dataLength];
    std::vector<Coord*> coords_;
    bool capture_;
};

#endif //SKIRMISH_GAME_TURN_H
