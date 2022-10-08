#include <iostream>
#include "doubutsushogi.h"

int main() {
  // no move states
  StateIndex idx = 1018420007456961;
  State s = make_state(idx);
  show_state(s);
  std::vector<Action> actions = valid_actions(s);
  std::cout << "Valid action counts for " << idx << ": " << actions.size() << std::endl;

  idx = 764207328431062;
  s = make_state(idx);
  show_state(s);
  actions = valid_actions(s);
  std::cout << "Valid action counts for " << idx << ": " << actions.size() << std::endl;
  
  return 0;
}