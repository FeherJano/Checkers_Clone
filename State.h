//
// Created by feher on 7/20/2022.
//

#ifndef SKIRMISH_GAME_STATE_H
#define SKIRMISH_GAME_STATE_H

#include <vector>
#include "Turn.h"
#include "Coord.h"

class Turn;


class State {
public:
    const static int boardSize = 8;
    const static int idIndex = boardSize * boardSize;
    const static int dataLength = idIndex + 1 + 1;

    State();
    ~State() {}

    void BuildListValidTurns();
    void Print() const;

    bool CheckMatchingVaildTurn(Turn* turn) const;
    void Move(Turn* turn);
    void SetNextPlayer();
    void SetInvalidTurn();

    char get_player() const;
    int get_winner_code() const;

    char* data() { return data_; }

private:
    const static int manKingDiff = 'a' - 'A';
    const static char whiteSpace = '_';
    const static char blackSpace = ' ';
    const static char validPlayerO = 'o';
    const static char validPlayerX = 'x';
    const static char invalidPlayerO = 'p';
    const static char invalidPlayerX = 'y';
    const static char winningPlayerO = 'O';
    const static char winningPlayerX = 'X';

    void CheckValidTurns(Coord* coord);

    bool CheckValidJumpTurns(
            Coord* pre_coord, Coord* new_coord, std::vector<Coord*> coords, bool king);
    void CheckValidMoveTurns(Coord* coord);

    void AddValidJumpTurn(const std::vector<Coord*> coords);
    void AddValidMoveTurn(Coord* pre_coord, Coord* new_coord);

    bool is_valid_coord(const Coord* coord) const;
    bool is_own_piece(const Coord* coord) const;
    bool is_opponent_piece(const Coord* coord) const;
    bool is_legal(const Coord* coord) const;
    bool is_king(const Coord* coord) const;

    bool is_valid_direction(Coord* pre_coord, Coord* new_coord) const;
    bool is_moveable(Coord* pre_coord, Coord* new_coord) const;
    bool is_jumpable(Coord* pre_coord, Coord* new_coord, bool king) const;

    char get_opponent() const;

    void set_player(const char player);

    void set_piece(const Coord* coord, const char piece);
    char get_piece(const Coord* coord) const;

    char data_[dataLength];
    std::vector<Turn*> valid_turns_;
    bool forced_capture_;
    int P1_unit_count;
    int P2_unit_count;
};

#endif //SKIRMISH_GAME_STATE_H
