//
// Created by feher on 7/20/2022.
//

#include "state.h"

#include <iostream>


State::State() : P1_unit_count(0), P2_unit_count(0) {
    int index = 0;

    // initialize board
    for (int y = 1; y <= boardSize; ++y) {
        for (int x = 1; x <= boardSize; ++x) {
            if ((x & 1) == (y & 1)) {             // legal space
                if (y <= 3) {                       // player o
                    data_[index] = validPlayerO;
                    ++P1_unit_count;
                }
                else if (y >= 6) {                  // player x
                    data_[index] = validPlayerX;
                    ++P2_unit_count;
                }
                else {                              // empty space
                    data_[index] = whiteSpace;
                }
            }
            else {                                // illegal space
                data_[index] = blackSpace;
            }
            ++index;
        }
    }

    // player o moves first
    data_[index++] = validPlayerO;

    // end of string
    data_[index] = '\0';
}

void State::BuildListValidTurns() {
    // reset
    valid_turns_.clear();
    forced_capture_ = false;

    // check valid turns based on every own piece
    for (int y = 0; y < boardSize; ++y) {
        for (int x = 0; x < boardSize; ++x) {
            Coord* coord = new Coord(x, y);
            if (is_own_piece(coord)) {
                CheckValidTurns(coord);
            }
        }
    }

    // game over
    if (valid_turns_.size() == 0) {
        char opponent = get_opponent();
        set_player(opponent - manKingDiff);
    }

    // forced capture
    if (forced_capture_) {
        auto iterator = valid_turns_.begin();
        while (iterator != valid_turns_.end()) {
            if ((*iterator)->capture()) {               // keep jump turn
                ++iterator;
            }
            else {                                      // delete move turn
                iterator = valid_turns_.erase(iterator);
            }
        }
    }

    // show all valid turns
    //for (auto turn : valid_turns_) {
    //  std::cout << "-- valid turn: " << turn->data() << std::endl;
    //}
    //std::cout << std::endl;
}

void State::Print() const {
    const char kHRuler[] = "  a b c d e f g h";
    std::cout << kHRuler << "\n";
    int index = boardSize * boardSize - boardSize;
    for (int y = boardSize; y > 0; --y) {
        std::cout << y << " ";
        for (int x = 0; x < boardSize; ++x) {
            std::cout << data_[index++] << " ";
        }
        std::cout << y << "\n";
        index -= (boardSize << 1);
    }
    std::cout << kHRuler << std::endl;
}

bool State::CheckMatchingVaildTurn(Turn* turn) const {
    for (auto valid_turn : valid_turns_) {
        if (valid_turn->CheckMatching(turn)) {
            return true;
        }
    }
    return false;
}

void State::Move(Turn* turn) {
    std::vector<Coord*> coords = turn->coords();
    char player = get_player();
    char piece = get_piece(coords[0]);
    bool king = is_king(coords[0]);
    int last_coord_index = coords.size() - 1;

    set_piece(coords[0], whiteSpace);

    // if jump
    if (turn->capture()) {
        for (int i = 0; i < last_coord_index; ++i) {
            Coord* mid_coord = coords[i]->Average(coords[i + 1]);
            set_piece(coords[i], whiteSpace);
            set_piece(mid_coord, whiteSpace);
        }
    }

    // crowning
    if (player == validPlayerO && coords[last_coord_index]->y == boardSize - 1
        || player == validPlayerX && coords[last_coord_index]->y == 0) {
        if (!king) {
            piece -= manKingDiff;
        }
    }

    // last coord
    set_piece(coords[last_coord_index], piece);
}

void State::SetNextPlayer() {
    if (get_player() == validPlayerO) {
        data_[idIndex] = validPlayerX;
    }
    else {
        data_[idIndex] = validPlayerO;
    }
}

void State::SetInvalidTurn() {
    if (get_player() == validPlayerO) {
        data_[idIndex] = invalidPlayerO;
    }
    else {
        data_[idIndex] = invalidPlayerX;
    }
}

char State::get_player() const {
    if (data_[idIndex] == validPlayerO
        || data_[idIndex] == invalidPlayerO
        || data_[idIndex] == winningPlayerO) {
        return validPlayerO;
    }
    else {
        return validPlayerX;
    }
}

int State::get_winner_code() const {
    if (data_[idIndex] == winningPlayerO) {
        return 1;
    }
    if (data_[idIndex] == winningPlayerX) {
        return 2;
    }
    return 0;
}

void State::CheckValidTurns(Coord* coord) {
    std::vector<Coord*> coords;
    coords.push_back(coord);
    bool king = is_king(coord);
    Coord* new_coord;

    // jump
    new_coord = new Coord(coord->x - 2, coord->y - 2);    // left bottom
    CheckValidJumpTurns(coord, new_coord, coords, king);
    new_coord = new Coord(coord->x + 2, coord->y - 2);    // right bottom
    CheckValidJumpTurns(coord, new_coord, coords, king);
    new_coord = new Coord(coord->x - 2, coord->y + 2);    // left top
    CheckValidJumpTurns(coord, new_coord, coords, king);
    new_coord = new Coord(coord->x + 2, coord->y + 2);    // right top
    CheckValidJumpTurns(coord, new_coord, coords, king);

    // move
    if (!forced_capture_) {
        CheckValidMoveTurns(coord);
    }
}

bool State::CheckValidJumpTurns(
        Coord* pre_coord, Coord* new_coord, std::vector<Coord*> coords, bool king) {
    if (is_jumpable(pre_coord, new_coord, king)) {
        coords.push_back(new_coord);
        bool child = false;
        Coord* next_coord;

        // advanced jump
        next_coord = new Coord(new_coord->x - 2, new_coord->y - 2);   // left bottom
        if (!pre_coord->Equal(next_coord)) {
            child = CheckValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }
        next_coord = new Coord(new_coord->x + 2, new_coord->y - 2);   // right bottom
        if (!pre_coord->Equal(next_coord)) {
            child = CheckValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }
        next_coord = new Coord(new_coord->x - 2, new_coord->y + 2);   // left top
        if (!pre_coord->Equal(next_coord)) {
            child = CheckValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }
        next_coord = new Coord(new_coord->x + 2, new_coord->y + 2);   // right top
        if (!pre_coord->Equal(next_coord)) {
            child = CheckValidJumpTurns(new_coord, next_coord, coords, king) || child;
        }

        if (!child) {
            forced_capture_ = true;
            AddValidJumpTurn(coords);
        }
        return true;
    }
    else {
        return false;
    }
}

void State::CheckValidMoveTurns(Coord* coord) {
    Coord* new_coord;

    new_coord = new Coord(coord->x - 1, coord->y - 1);        // left bottom
    if (is_moveable(coord, new_coord)) {
        AddValidMoveTurn(coord, new_coord);
    }
    new_coord = new Coord(coord->x + 1, coord->y - 1);        // right bottom
    if (is_moveable(coord, new_coord)) {
        AddValidMoveTurn(coord, new_coord);
    }
    new_coord = new Coord(coord->x - 1, coord->y + 1);        // left top
    if (is_moveable(coord, new_coord)) {
        AddValidMoveTurn(coord, new_coord);
    }
    new_coord = new Coord(coord->x + 1, coord->y + 1);        // right top
    if (is_moveable(coord, new_coord)) {
        AddValidMoveTurn(coord, new_coord);
    }
}

void State::AddValidJumpTurn(const std::vector<Coord*> coords) {
    valid_turns_.push_back(new Turn(coords, true));
}

void State::AddValidMoveTurn(Coord* pre_coord, Coord* new_coord) {
    std::vector<Coord*> coords;
    coords.push_back(pre_coord);
    coords.push_back(new_coord);
    valid_turns_.push_back(new Turn(coords, false));
}

bool State::is_valid_coord(const Coord* coord) const {
    return (coord->x >= 0 && coord->x < boardSize
            && coord->y >= 0 && coord->y < boardSize);
}

bool State::is_own_piece(const Coord* coord) const {
    char player = get_player();
    char piece = get_piece(coord);

    if (piece == player || piece == player - manKingDiff) {
        return true;
    }
    else {
        return false;
    }
}

bool State::is_opponent_piece(const Coord* coord) const {
    char opponent = get_opponent();
    char piece = get_piece(coord);

    if (piece == opponent || piece == opponent - manKingDiff) {
        return true;
    }
    else {
        return false;
    }
}

bool State::is_legal(const Coord* coord) const {
    return get_piece(coord) == whiteSpace;
}

bool State::is_king(const Coord* coord) const {
    char piece = get_piece(coord);
    return piece >= 'A' && piece <= 'Z';
}

bool State::is_valid_direction(Coord* pre_coord, Coord* new_coord) const {
    char player = get_player();
    int stepY = new_coord->y - pre_coord->y;
    if (player == validPlayerO && stepY > 0
        || player == validPlayerX && stepY < 0) {
        return true;
    }
    return false;
}

bool State::is_moveable(Coord* pre_coord, Coord* new_coord) const {
    if (is_valid_coord(new_coord) && is_legal(new_coord)
        && (is_valid_direction(pre_coord, new_coord) || is_king(pre_coord))) {
        return true;
    }
    return false;
}

bool State::is_jumpable(Coord* pre_coord, Coord* new_coord, bool king) const {
    if (is_valid_coord(new_coord) && is_legal(new_coord)
        && (is_valid_direction(pre_coord, new_coord) || king)) {
        Coord* mid_coord = pre_coord->Average(new_coord);
        if (is_opponent_piece(mid_coord)) {
            return true;
        }
    }
    return false;
}

char State::get_opponent() const {
    char player = get_player();
    if (player == validPlayerO) {
        return validPlayerX;
    }
    else {
        return validPlayerO;
    }
}

void State::set_player(const char player) {
    data_[idIndex] = player;
}

void State::set_piece(const Coord* coord, const char piece) {
    data_[coord->y * boardSize + coord->x] = piece;
}

char State::get_piece(const Coord* coord) const {
    return data_[coord->y * boardSize + coord->x];
}