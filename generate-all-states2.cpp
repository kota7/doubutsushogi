#include <iostream>
#include <fstream>
#include <set>
#include <unordered_set>
#include <vector>
#include <chrono>
#include "doubutsushogi.h"

/*
Genarate all states reacheable from the initial state.

Output is the lines of the following format:

<state index> <status> <number of next states> <next state index 1> ...

Conditions:
  States are always from the first player's point of view.
  States are aggregated to horizontal flips (mirror positions).
  If a mover can win by one move, then this state is marked as WIN.
  In this version, we still include the continuation states so that we can evaluate all legal moves.
*/


void write_line(StateIndex idx, bool win, const std::set<StateIndex> &next_state_indices, std::ofstream &outfile) {
  outfile << idx << ' ' << win << ' ' << next_state_indices.size();
  for (std::set<StateIndex>::iterator it=next_state_indices.begin(); it != next_state_indices.end(); ++it) {
    outfile << ' ' << *it;
  }
  outfile << std::endl;
}

void write_line(StateIndex idx, bool win, std::ofstream &outfile) {
  outfile << idx << ' ' << win << ' ' << 0 << std::endl;
}


int main() {
  // keep track the state indices already added to the checklist to avoid repetitions
  std::unordered_set<StateIndex> covered;
  
  // keep track the state indices to expand
  // shoud we use unordered set for this?
  std::vector<StateIndex> tocheck;
  StateIndex i = normalize_state(init_state());
  tocheck.push_back(i);
  covered.insert(i);

  // keep track the compuation speed
  auto starttime = std::chrono::steady_clock::now();

  // output file
  std::ofstream outfile("doubutsu-states-full.txt");
  std::ofstream outfile_win("doubutsu-states-win.txt");
  size_t n_recorded = 0;
  while (tocheck.size() > 0) {
    // progress report
    if (n_recorded % 1000000 == 0) {
      auto thistime = std::chrono::steady_clock::now();
      std::chrono::duration<double> elapsed_seconds = thistime - starttime;
      float minute_per_10m = (float)elapsed_seconds.count()/60 / ((float)n_recorded/1000/1000/10);
      std::cerr << "\rRecorded: " << n_recorded
        << ", Covered " << covered.size()
        << ", To check " << tocheck.size()
        << ", Minute-per-10M " << minute_per_10m;
    }

    StateIndex idx = tocheck.back();
    tocheck.pop_back();
    State s = make_state(idx);

    bool win = false;
    std::set<StateIndex> next_state_indices;
    std::vector<Action> actions = valid_actions(s);
    for (std::vector<Action>::iterator it=actions.begin(); it != actions.end(); ++it) {
      std::pair<State, bool> result = take_action(s, *it);
      if (result.second) {
        // we can win from this state in one move
        // this state does not need to explore further
        win = true;
        //next_state_indices.clear();
        //break;
      } else {
        // we add non-winning states as the continuation
        // Here, we omit the state where the lion is already tried.
        // This happens if the previous move misses to capture the opponent lion
        // Although not programmed explicitly, this will end the game
        // Hence this is effectively a check for additional terminal condition
        bool tried = false;
        for (size_t i=0; i<3; i++) {
          if (result.first[i] == LION) {
            tried = true;
            break;
          }
        }
        if (!tried) {
          // add this continuation value
          StateIndex idx2 = normalize_state(result.first);
          next_state_indices.insert(idx2);
        }
      }
    }
    // record this line
    if (win) {
      write_line(idx, win, outfile_win);
      n_recorded++;
    } else {
      write_line(idx, win, next_state_indices, outfile);
      n_recorded++;
    }

    // add continuation states to the check list
    for (std::set<StateIndex>::iterator it=next_state_indices.begin(); it != next_state_indices.end(); ++it) {
      // Add this index to the checklist only if it has not been covered yet
      // and mark as covered immediately
      // This way, we ensure that each index is recorded exactly once
      if (covered.find(*it) == covered.end()) {
        tocheck.push_back(*it);
        covered.insert(*it);  // mark as covered because we have this in the checklist now
      }
    }
  }

  outfile.close();
  outfile_win.close();

  // final report
  auto thistime = std::chrono::steady_clock::now();
  std::chrono::duration<double> elapsed_seconds = thistime - starttime;
  std::cerr << std::endl << "Finished." << std::endl
    << "Recorded: " << n_recorded
    << ", Covered " << covered.size()
    << ", To check " << tocheck.size()
    << ", Elapsed minute " << elapsed_seconds.count() / 60
    << std::endl;

  return 0;
}