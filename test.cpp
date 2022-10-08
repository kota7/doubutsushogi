#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "doubutsushogi.h"


void require(bool condition, std::string message) {
  if (!condition) {
    std::cout << "!ERROR: " << message << std::endl << std::flush;
    std::cin.get();
  } else {
    std::cout << "OK!" << std::endl;
  }
}

void require(bool condition) {
  require(condition, "");
}

void test_state_index(const State &s) {  
  StateIndex i1 = state_index(s);
  StateIndex i2 = state_index(make_state(i1));
  std::cout << i1 << " vs " << i2 << std::endl;
  require (i1 == i2);
}

void random_game() {
  srand((unsigned int)time(NULL));
  std::cout << "Start random game" << std::endl;
  State s = init_state();
  for (int i=0; i<100; i++) {
    std::cout << "Period " << i << std::endl;
    show_state(s);
    std::cout << "Testing index validity" << std::endl;
    test_state_index(s);

    std::vector<Action> actions = valid_actions(s);
    std::cout << "Valid actions in the init state:" << std::endl;
    for (std::vector<Action>::iterator it=actions.begin(); it != actions.end(); ++it) {
        std::cout << it->first << "->" << it->second << " ";
    }
    std::cout << std::endl;
    int idx = rand() % actions.size();
    std::cout << "Action index " << idx << std::endl;
    Action a = actions[idx];
    std::cout << "Action: " << a.first << " " << a.second << std::endl;
    std::pair<State, bool> result = take_action(s, a);
    
    if (result.second) {
      std::cout << "Game over!" << std::endl;
      std::cout << "Final state:" << std::endl;
      show_state(result.first);
      break;
    }
    s = result.first;
  }    
}

bool is_valid_action(const State &s, const Action &a) {
  std::vector<Action> actions = valid_actions(s);
  bool out = (std::find(actions.begin(), actions.end(), a) != actions.end());
  return out;
}

void unit_tests() {
  std::cout << "Init position test" << std::endl; 
  State expected = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, HIYOKO, EMPTY, ZOU, LION, KIRIN, 0, 0, 0, 0, 0, 0};
  State s = init_state();
  show_state(s); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (s == expected);
  std::cout << "OK" << std::endl;

  std::cout << "Test moving KIRIN forward" << std::endl;
  Action a = std::make_pair(11, 8);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  std::pair<State, bool> result = take_action(s, a);
  expected = {EMPTY, -LION, -ZOU, -KIRIN, -HIYOKO, EMPTY, EMPTY, HIYOKO, EMPTY, ZOU, LION, KIRIN, 0, 0, 0, 0, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Test capturing HIYKO" << std::endl;
  a = std::make_pair(7, 4);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {-KIRIN, -LION, -ZOU, EMPTY, EMPTY, EMPTY, EMPTY, -HIYOKO, EMPTY, ZOU, LION, KIRIN, 0, 0, 0, 1, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Test moving LION" << std::endl;
  a = std::make_pair(10, 6);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {-KIRIN, EMPTY, -ZOU, EMPTY, -HIYOKO, -LION, EMPTY, HIYOKO, EMPTY, ZOU, LION, KIRIN, 0, 0, 0, 0, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Test capturing LION by KIRIN" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, EMPTY, -HIYOKO, -LION, EMPTY, HIYOKO, KIRIN, ZOU, LION, EMPTY, 0, 0, 0, 0, 0, 0};
  a = std::make_pair(8, 5);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, -KIRIN, HIYOKO, EMPTY, ZOU, EMPTY, KIRIN, 0, 0, 0, 0, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Test capturing LION by ZOU" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, -LION, -HIYOKO, EMPTY, EMPTY, ZOU, KIRIN, EMPTY, LION, EMPTY, 1, 0, 0, 0, 0, 0};
  a = std::make_pair(7, 3);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -LION, EMPTY, -KIRIN, EMPTY, EMPTY, EMPTY, HIYOKO, -ZOU, ZOU, EMPTY, KIRIN, 0, 0, 0, 1, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Use captured HIYOKO" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, -LION, -HIYOKO, EMPTY, EMPTY, ZOU, KIRIN, EMPTY, LION, EMPTY, 1, 0, 0, 0, 0, 0};
  a = std::make_pair(12, 5);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -LION, EMPTY, -KIRIN, -ZOU, EMPTY, -HIYOKO, HIYOKO, LION, ZOU, EMPTY, KIRIN, 0, 0, 0, 0, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "HIYOKO promotion" << std::endl;
  s = {EMPTY, -KIRIN, -ZOU, -LION, HIYOKO, EMPTY, EMPTY, ZOU, KIRIN, EMPTY, LION, EMPTY, 0, 0, 0, 1, 0, 0};
  a = std::make_pair(4, 1);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -LION, EMPTY, -KIRIN, -ZOU, EMPTY, EMPTY, EMPTY, LION, ZOU, -TORI, EMPTY, 1, 0, 0, 0, 0, 1};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Use captured ZOU" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, -LION, -HIYOKO, EMPTY, EMPTY, EMPTY, KIRIN, EMPTY, LION, EMPTY, 1, 1, 0, 0, 0, 0};
  a = std::make_pair(13, 9);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -LION, -ZOU, -KIRIN, EMPTY, EMPTY, EMPTY, HIYOKO, LION, ZOU, EMPTY, KIRIN, 0, 0, 0, 1, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Use captured KIRIN" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, -LION, -HIYOKO, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, LION, EMPTY, 1, 1, 1, 0, 0, 0};
  a = std::make_pair(14, 1);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -LION, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, HIYOKO, LION, ZOU, -KIRIN, KIRIN, 0, 0, 0, 1, 1, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Try success" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, EMPTY, -HIYOKO, LION, -LION, EMPTY, EMPTY, EMPTY, ZOU, EMPTY, 0, 0, 0, 1, 0, 1};
  a = std::make_pair(5, 2);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -ZOU, EMPTY, EMPTY, EMPTY, LION, EMPTY, HIYOKO, EMPTY, -LION, EMPTY, KIRIN, 1, 0, 1, 0, 1, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Try failure" << std::endl;
  s = {-KIRIN, EMPTY, -ZOU, EMPTY, -HIYOKO, LION, -LION, EMPTY, EMPTY, EMPTY, ZOU, EMPTY, 0, 0, 0, 1, 0, 1};
  a = std::make_pair(5, 1);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {EMPTY, -ZOU, EMPTY, EMPTY, EMPTY, LION, EMPTY, HIYOKO, EMPTY, ZOU, -LION, KIRIN, 1, 0, 1, 0, 0, 0};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;

  std::cout << "Init state valid actions" << std::endl;
  s = init_state();
  std::vector<Action> actions = valid_actions(s);
  require (actions.size() == 4);
  require (is_valid_action(s, std::make_pair(7, 4)));  // hiyoko forward
  require (is_valid_action(s, std::make_pair(11, 8)));  // kirin forward
  require (is_valid_action(s, std::make_pair(10, 8)));  // lion right
  require (is_valid_action(s, std::make_pair(10, 6)));  // lion left
  std::cout << "OK" << std::endl;

  std::cout << "Many available actions" << std::endl;
  s = {EMPTY, EMPTY, -LION, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, LION, EMPTY, 1, 1, 1, 1, 1, 1};
  require (is_valid_action(s, std::make_pair(12, 0)));
  require (is_valid_action(s, std::make_pair(12, 1)));
  require (!is_valid_action(s, std::make_pair(12, 2)));
  require (is_valid_action(s, std::make_pair(12, 3)));
  require (is_valid_action(s, std::make_pair(12, 4)));
  require (is_valid_action(s, std::make_pair(12, 5)));
  require (is_valid_action(s, std::make_pair(12, 6)));
  require (is_valid_action(s, std::make_pair(12, 7)));
  require (is_valid_action(s, std::make_pair(12, 8)));
  require (is_valid_action(s, std::make_pair(12, 9)));
  require (!is_valid_action(s, std::make_pair(12, 10)));
  require (is_valid_action(s, std::make_pair(12, 11)));
  require (is_valid_action(s, std::make_pair(13, 0)));
  require (is_valid_action(s, std::make_pair(13, 1)));
  require (!is_valid_action(s, std::make_pair(13, 2)));
  require (is_valid_action(s, std::make_pair(13, 3)));
  require (is_valid_action(s, std::make_pair(13, 4)));
  require (is_valid_action(s, std::make_pair(13, 5)));
  require (is_valid_action(s, std::make_pair(13, 6)));
  require (is_valid_action(s, std::make_pair(13, 7)));
  require (is_valid_action(s, std::make_pair(13, 8)));
  require (is_valid_action(s, std::make_pair(13, 9)));
  require (!is_valid_action(s, std::make_pair(13, 10)));
  require (is_valid_action(s, std::make_pair(13, 11)));
  require (is_valid_action(s, std::make_pair(14, 0)));
  require (is_valid_action(s, std::make_pair(14, 1)));
  require (!is_valid_action(s, std::make_pair(14, 2)));
  require (is_valid_action(s, std::make_pair(14, 3)));
  require (is_valid_action(s, std::make_pair(14, 4)));
  require (is_valid_action(s, std::make_pair(14, 5)));
  require (is_valid_action(s, std::make_pair(14, 6)));
  require (is_valid_action(s, std::make_pair(14, 7)));
  require (is_valid_action(s, std::make_pair(14, 8)));
  require (is_valid_action(s, std::make_pair(14, 9)));
  require (!is_valid_action(s, std::make_pair(14, 10)));
  require (is_valid_action(s, std::make_pair(14, 11)));
  // out of board
  require (!is_valid_action(s, std::make_pair(12, -1)));
  require (!is_valid_action(s, std::make_pair(12, 12)));
  require (!is_valid_action(s, std::make_pair(13, -1)));
  require (!is_valid_action(s, std::make_pair(13, 12)));
  require (!is_valid_action(s, std::make_pair(14, -1)));
  require (!is_valid_action(s, std::make_pair(14, 12)));
  std::cout << "OK" << std::endl;

  std::cout << "Moving HIYOKO" << std::endl;
  s = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, HIYOKO, EMPTY, ZOU, LION, KIRIN, 0, 0, 0, 0, 0, 0};
  show_state(s);
  for (unsigned short i=0; i < 12; i++) {
    std::cout << i << ' ';
    if (i == 4) {
      require (is_valid_action(s, std::make_pair(7, i)), "HIYOKO 7 -> " + std::to_string(i));
    } else {
      require (!is_valid_action(s, std::make_pair(7, i)), "HIYOKO 7 -> " + std::to_string(i));
    }
  }
  std::cout << std::endl;
  std::cout << "OK" << std::endl;

  std::cout << "Moving ZOU" << std::endl;
  s = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, ZOU, EMPTY, EMPTY, LION, EMPTY, 1, 1, 0, 0, 0, 0};
  show_state(s);
  for (unsigned short i=0; i < 12; i++) {
    std::cout << i << ' ';
    if (i == 3 || i == 5 || i == 9 || i == 11) {
      require (is_valid_action(s, std::make_pair(7, i)), "ZOU 7 -> " + std::to_string(i));
    } else {
      require (!is_valid_action(s, std::make_pair(7, i)), "ZOU 7 -> " + std::to_string(i));
    }
  }
  std::cout << std::endl;
  std::cout << "OK" << std::endl;

  std::cout << "Moving KIRIN" << std::endl;
  s = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, KIRIN, EMPTY, EMPTY, EMPTY, LION, 1, 0, 0, 0, 0, 0};
  show_state(s);
  for (unsigned short i=0; i < 12; i++) {
    std::cout << i << ' ';
    if (i == 4 || i == 6 || i == 8 || i == 10) {
      require (is_valid_action(s, std::make_pair(7, i)), "KIRIN 7 -> " + std::to_string(i));
    } else {
      require (!is_valid_action(s, std::make_pair(7, i)), "KIRIN 7 -> " + std::to_string(i));
    }
  }
  std::cout << std::endl;
  std::cout << "OK" << std::endl;

  std::cout << "Moving LION" << std::endl;
  s = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, LION, EMPTY, EMPTY, EMPTY, EMPTY, 1, 1, 1, 0, 0, 0};
  show_state(s);
  for (unsigned short i=0; i < 12; i++) {
    std::cout << i << ' ';
    if (i == 3 || i == 4 || i == 5 || i == 6 || i == 8 || i == 9 || i == 10 || i == 11) {
      require (is_valid_action(s, std::make_pair(7, i)), "LION 7 -> " + std::to_string(i));
    } else {
      require (!is_valid_action(s, std::make_pair(7, i)), "LION 7 -> " + std::to_string(i));
    }
  }
  std::cout << std::endl;
  std::cout << "OK" << std::endl;

  std::cout << "Moving TORI" << std::endl;
  s = {-KIRIN, -LION, -ZOU, EMPTY, -HIYOKO, EMPTY, EMPTY, TORI, EMPTY, LION, EMPTY, EMPTY, 1, 1, 1, 0, 0, 0};
  show_state(s);
  for (unsigned short i=0; i < 12; i++) {
    std::cout << i << ' ';
    if (i == 3 || i == 4 || i == 5 || i == 6 || i == 8 || i == 10) {
      require (is_valid_action(s, std::make_pair(7, i)), "TORI 7 -> " + std::to_string(i));
    } else {
      require (!is_valid_action(s, std::make_pair(7, i)), "TORI 7 -> " + std::to_string(i));
    }
  }
  std::cout << std::endl;
  std::cout << "OK" << std::endl;

  std::cout << "Use one of two pieces" << std::endl;
  s = {EMPTY, -LION, -ZOU, -HIYOKO, -HIYOKO, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, ZOU, LION, 0, 0, 2, 0, 0, 0};
  require (!is_valid_action(s, std::make_pair(14, 10)));
  require (!is_valid_action(s, std::make_pair(14, 11)));
  a = std::make_pair(14, 9);
  require (is_valid_action(s, a));
  show_state(s); std::cout << std::endl << "Action: " << a.first << "->" << a.second << std::endl;
  result = take_action(s, a);
  expected = {-LION, -ZOU, -KIRIN, EMPTY, EMPTY, EMPTY, EMPTY, HIYOKO, HIYOKO, ZOU, LION, EMPTY, 0, 0, 0, 0, 0, 1};
  show_state(result.first); std::cout << std::endl << " vs" << std::endl; show_state(expected); std::cout << std::endl;  
  require (result.first == expected);
  require (!result.second);
  std::cout << "OK" << std::endl;  

  std::cout << "Normalization" << std::endl;
  State s1 = {EMPTY, -LION, -ZOU, -HIYOKO, -HIYOKO, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, ZOU, LION, 0, 0, 2, 0, 0, 0};
  State s2 = {-ZOU, -LION, EMPTY, EMPTY, -HIYOKO, -HIYOKO, EMPTY, EMPTY, EMPTY, LION, ZOU, EMPTY, 0, 0, 2, 0, 0, 0};
  State s3 = {-ZOU, -LION, EMPTY, EMPTY, -HIYOKO, -HIYOKO, EMPTY, EMPTY, EMPTY, LION, ZOU, EMPTY, 0, 0, 1, 0, 0, 1};
  require (normalize_state(s1) == normalize_state(s2));
  require (normalize_state(s1) != normalize_state(s3));
}

void show_state_examples() {
  std::vector<StateIndex> indices = {
    2146939695970, 2148001833970, 5415026403812, 564767460557, 89582008669948, 343747411826987, 852216635503458,
    343747411826987, 90312972454158, 89527745756518, 343794570780897, 491164137587436, 1621000615322445, 397145583654737
  };
  for (size_t i=0; i<indices.size(); i++) {
    std::cout << "Winning state " << i+1 << std::endl;
    StateIndex idx = indices[i];
    State s = make_state(idx);
    std::cout << "State index " << idx << std::endl;
    show_state(s);
    std::cin.get();
  }

  indices = {
    117825525606690, 852217707253918, 117825472459810, 852665717355378, 118274120282750, 852217706814738, 117703535916420,
    2276617162570, 2276617484980, 2276617961170, 2146900741538, 5415026081402, 2147963518418, 5415026880002, 5285437170170
  };
  for (size_t i=0; i<indices.size(); i++) {
    std::cout << "Non-winning state " << i+1 << std::endl;
    StateIndex idx = indices[i];
    State s = make_state(idx);
    std::cout << "State index " << idx << std::endl;
    show_state(s);
    std::cin.get();
  }

}


int main() {
  
  State s = init_state();
  show_state(s);
  unsigned long long idx = state_index(s);
  std::cout << "Index of init state " << idx << std::endl;

  State s2 = make_state(idx);
  std::cout << "Re-created init state: " << std::endl;
  show_state(s2);
  unsigned long long idx2 = state_index(s2);
  std::cout << "Index of re-created init state " << idx2 << std::endl;
  require (idx == idx2);

  State s3 = flip_mover(s);
  std::cout << "User-flipped init state: " << std::endl;
  show_state(s3);
  unsigned long long idx3 = state_index(s3);
  std::cout << "Index of user-flipped init state " << idx3 << std::endl;
  require (idx == idx3);

  std::vector<Action> actions = valid_actions(s);
  std::cout << "Valid actions in the init state:" << std::endl;
  for (std::vector<Action>::iterator it=actions.begin(); it != actions.end(); ++it) {
    std::cout << it->first << " " << it->second << std::endl;
  }

  for (int i=0; i< 100; i++) {
    std::cout << "Start game " << i+1 << std::endl;
    random_game();
    std::cout << "End game " << i+1 << std::endl;
  }

  std::cout << "Start unit tests" << std::endl;
  unit_tests();
  std::cout << "End unit tests" << std::endl;

  std::cout << "Start checking state examples" << std::endl;
  show_state_examples();
  std::cout << "End checking state examples" << std::endl;

  return 0;
}
