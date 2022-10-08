#ifndef DOUBUTSUSHOGIHEADERDEF
#define DOUBUTSUSHOGIHEADERDEF


#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

/*
State representation.

We repesent a state by a 18-vector of integers.
The first 12 represent the board, the last 6 represent prisoners of two players

A board is ordered rowwise.

empty = 0, hiyoko = 1, zou = 2, kirin = 3, niwatori = 4, lion = 5.
second player pieces are the negatives.
prisoners are ordered as (hiyoko, zou, kirin)

Mover is always the first player.

This way, the numeber of possible states are bounded by 11^12 * 3^6 < 2^52.
So we can define a state index representing the state.


Actions.

We represent an action by a pair of integers (from_index, to_index).
  from_index: The position of the piece to move from. If using a prisoner, that index is 11 + piece.
              Possible values are {0, ..., 14}.
  to_index: The position of the piece to move to. Possible values are {0, ..., 11}
*/

using State = std::vector<short>;
using StateIndex = unsigned long long;
using Action = std::pair<unsigned short, unsigned short>;

const short EMPTY = 0;
const short HIYOKO = 1;
const short ZOU = 2;
const short KIRIN = 3;
const short TORI = 4;
const short LION = 5;
const std::string PIECE_REPR = "ltkzh.HZKTL";  // can use by indexing with piece+5

State flip_mover(const State &s);
StateIndex state_index(const State &s);
State make_state(StateIndex index);
StateIndex normalize_state(const State &s);
State init_state();
void show_state(const State &s);
void show_state(StateIndex index);
std::vector<unsigned short> next_positions(short p, unsigned short idx);
std::vector<Action> valid_actions(const State &s) ;
std::pair<State, bool> take_action(const State &s, const Action &a);

#endif