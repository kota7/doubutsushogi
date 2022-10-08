#include "doubutsushogi.h"

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

/*
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
*/

State flip_mover(const State &s) {
  State out;
  out.resize(18);
  for (size_t i=0; i<12; i++) out[11-i] = -s[i];
  for (size_t i=12; i<15; i++) out[i] = s[i+3];
  for (size_t i=15; i<18; i++) out[i] = s[i-3];
  return out;
}

StateIndex state_index(const State &s) {
  StateIndex out = 0;
  StateIndex base = 1; 
  for (size_t i=0; i<12; i++) {
    out += base * (s[i] + 5);
    base *= 11;
  }
  for (size_t i=12; i<18; i++) {
    out += base * s[i];
    base *= 3;
  }
  return out;
}

State make_state(StateIndex index) {
  State out;
  out.resize(18);
  for (size_t i=0; i<12; i++) {
    out[i] = (index % 11) - 5;
    index /= 11;
  }
  for (size_t i=12; i<18; i++) {
    out[i] = index % 3;
    index /= 3;
  }
  return out;
}

StateIndex normalize_state(const State &s) {
  StateIndex idx = state_index(s);

  // make an mirror state and return the smaller index
  State s2 = s;
  std::swap(s2[0], s2[2]);
  std::swap(s2[3], s2[5]);
  std::swap(s2[6], s2[8]);
  std::swap(s2[9], s2[11]);
  StateIndex idx2 = state_index(s2);
  if (idx2 < idx) idx = idx2;
  return idx;
}

State init_state() {
  State out = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, HIYOKO, EMPTY, ZOU, LION, KIRIN, 0, 0, 0, 0, 0, 0};
  return out;
}

void show_state(const State &s) {
  for (size_t i=0; i<s.size(); i++) std::cout << s[i] << " ";
  std::cout << std::endl;

  std::cout << " ------- " << std::endl;
  for (size_t i=0; i<12; i++) {
    if (i % 3 == 0) std::cout << "|";
    std::cout << ' ' << PIECE_REPR[s[i]+5];
    if (i % 3 == 2) std::cout << " |" << std::endl;
  }
  std::cout << " ------- " << std::endl;
  for (size_t i=12; i<15; i++) std::cout << ' ' << PIECE_REPR[i-6] << ':' << s[i];
  std::cout << std::endl;
  for (size_t i=15; i<18; i++) std::cout << ' ' << PIECE_REPR[19-i] << ':' << s[i];
  std::cout << std::endl;
}

void show_state(StateIndex index) {
  show_state(make_state(index));
}

/*
ActionIndex action_index(const Action &a) {
  return a.first * 12 + a.second
} 

Action make_action(ActionIndex index) {
  Action a = std::make_pair(index / 12, index % 12);
}
*/

std::vector<unsigned short> next_positions(short p, unsigned short idx) {
  // returns the indices where the piece p can go from the idx
  // supports the first mover only
  std::vector<unsigned short> out;
  if (p == HIYOKO) {
    if (idx >= 3) out.push_back(idx-3);
  } else if (p == ZOU) {
    if (idx >= 3) {
      if (idx % 3 != 0) out.push_back(idx-4);
      if (idx % 3 != 2) out.push_back(idx-2);
    }
    if (idx < 9) {
      if (idx % 3 != 0) out.push_back(idx+2);
      if (idx % 3 != 2) out.push_back(idx+4);
    }
  } else if (p == KIRIN) {
    if (idx % 3 != 0) out.push_back(idx-1);
    if (idx % 3 != 2) out.push_back(idx+1);
    if (idx >= 3) out.push_back(idx-3);
    if (idx < 9) out.push_back(idx+3);
  } else if (p == LION) {
    if (idx % 3 != 0) out.push_back(idx-1);
    if (idx % 3 != 2) out.push_back(idx+1);
    if (idx >= 3) {
      out.push_back(idx-3);
      if (idx % 3 != 0) out.push_back(idx-4);
      if (idx % 3 != 2) out.push_back(idx-2);
    }
    if (idx < 9) {
      out.push_back(idx+3);
      if (idx % 3 != 0) out.push_back(idx+2);
      if (idx % 3 != 2) out.push_back(idx+4);
    }
  } else if (p == TORI) {
    if (idx % 3 != 0) out.push_back(idx-1);
    if (idx % 3 != 2) out.push_back(idx+1);
    if (idx >= 3) {
      out.push_back(idx-3);
      if (idx % 3 != 0) out.push_back(idx-4);
      if (idx % 3 != 2) out.push_back(idx-2);
    }
    if (idx < 9) out.push_back(idx+3);
  }
  return out;
}

std::vector<Action> valid_actions(const State &s) {
  std::vector<Action> out;
  // use a piece on the board
  for (unsigned short i=0; i<12; i++) {
    if (s[i] > 0) {
      std::vector<unsigned short> targets = next_positions(s[i], i);
      for (std::vector<unsigned short>::iterator it=targets.begin(); it != targets.end(); ++it) {
        if (s[*it] <= 0) out.push_back(std::make_pair(i, *it));
      }
    }
  }
  // use a prisoner piece
  std::vector<unsigned short> empties;
  for (unsigned short i=0; i<12; i++) {
    if (s[i] == EMPTY) empties.push_back(i);
  }
  for (unsigned short i=12; i<15; i++) {
    if (s[i] > 0) {
      for (std::vector<unsigned short>::iterator it=empties.begin(); it != empties.end(); ++it) {
        out.push_back(std::make_pair(i, *it));
      }
    } 
  }
  return out;
}


std::pair<State, bool> take_action(const State &s, const Action &a) {
  // returns the resulted state and the status
  // support first player only
  // Technically, we may lose by missing the trying opponent lion by this move
  // This extension is to be considered
  // But such move should not affect the state values
  State out = s;  // make a copy
  bool win = false;
  if (a.first < 12) {
    // use a piece on the board
    if (out[a.second] < 0 and out[a.second] != -LION) out[12+(-out[a.second]-1) % 3]++;  // add prisoner

    out[a.second] = s[a.first];
    out[a.first] = EMPTY;
    // promotion to tori
    if (a.second < 3 && out[a.second] == HIYOKO) out[a.second] = TORI;
    out = flip_mover(out);
    
    // check if the mover wins
    if (s[a.second] == -LION) {
      win = true;  // captured lion
    } else if (s[a.first] == LION && a.second < 3) {
      // LION has reached to end row, so check for try
      // using the user-flipped board, we check if the -LION can be captured by the first player
      unsigned short lion_idx = 11 - a.second;  // lion location in the flipped board
      std::vector<unsigned short> neighbors;
      neighbors.push_back(lion_idx-3);
      if (lion_idx != 0) {
        neighbors.push_back(lion_idx-1);
        neighbors.push_back(lion_idx-4);
      }      
      if (lion_idx != 2) {
        neighbors.push_back(lion_idx+1);
        neighbors.push_back(lion_idx-2);
      }
      bool istry = true;
      for (std::vector<unsigned short>::iterator it=neighbors.begin(); it != neighbors.end(); ++it) {
        if (out[*it] > 0) {
          std::vector<unsigned short> attacked = next_positions(out[*it], *it);
          if (std::find(attacked.begin(), attacked.end(), lion_idx) != attacked.end()) {
            // lion_idx is in an attacked position, so try failed
            istry = false;
            break;
          }
        }
      }
      if (istry) win = true;
    }    
  } else {
    // use a prisoner
    out[a.second] = a.first - 11;
    out[a.first]--;
    // cannot win by using a prisoner, so not need check if the mover win
    out = flip_mover(out);
  }

  // TBC: Add possible check for the missing tying lion
  return std::make_pair(out, win);
}

