#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

// We redefine this type here
// to make this script independent of the doubutsushogi.h
using StateIndex = unsigned long long;


struct StateInfo {
  float value;
  std::vector<StateIndex> next_state_indices;
};

void load_data(std::unordered_map<StateIndex, StateInfo> &states) {
  std::ifstream infile("doubutsu-states-full.txt");
  std::string line;
  size_t n_lines = 0;
  while ( std::getline(infile, line) ) {
    n_lines++;
    if (n_lines % 1000000 == 0) std::cerr << "\rReading line " << n_lines << ", n. states = " << states.size();
    std::istringstream stream(line);

    StateIndex idx;
    bool win;
    size_t k;
    stream >> idx >> win >> k;
    // We should not have any state with win=1 in this file
    // Just in case we have one, we warn and skip the line
    if (win) {
      std::cerr << "WARN: Win should be zero but in the line " << n_lines << " " << idx << std::endl;
      continue;
    }

    StateInfo info;
    info.value = 0;
    info.next_state_indices.resize(k);
    for (size_t i=0; i<k; i++) stream >> info.next_state_indices[i];
    /*
    if (info.next_state_indices.size() == 0) {
      std::cerr << "Line " << n_lines << ", Index " << idx << " has no continuation states" << std::endl << std::flush;
      assert(false);
    }
    */
    states[idx] = info;
  }
  infile.close();
  // If the file does not contain states with win=1, then the number of lines and the number of states should equal
  std::cerr << std::endl << n_lines << " lines read, " << states.size() << " states remaining" << std::endl;
}

float get_value(const std::unordered_map<StateIndex, StateInfo> &states, StateIndex idx) {
  auto it = states.find(idx);
  // if this state is not in the info, this is a winning state.
  if (it == states.end()) return 1;
  return (it->second).value;
}

float update_value(std::unordered_map<StateIndex, StateInfo> &states, StateInfo &info, float gamma) {
  // Edge case: if there is no continuation states (i.e. no valid actions), then we assume this mover loses (like in chess)
  //            so the value of this state is -1.
  // Note that this does not happen unless movers misses one-step wins
  if (info.next_state_indices.size() == 0) {
    float deviation = std::abs(info.value + 1);
    info.value = -1;
    return deviation;
  }

  // Find the minimum continuation value (least beneficial to the opponent)
  // Then, its negative (times discount factor) becomes the new value for this index
  float new_value = 999;
  for (std::vector<StateIndex>::iterator jt=info.next_state_indices.begin(); jt != info.next_state_indices.end(); ++jt) {
    float tmp = get_value(states, *jt);
    if (tmp < new_value) new_value = tmp;
  }
  /*
  if (abs(new_value) > 1) {
    std::cerr << "New value is not in [1,-1] " << info.next_state_indices.size() << " continuation states: ";
    for (std::vector<StateIndex>::iterator jt=info.next_state_indices.begin(); jt != info.next_state_indices.end(); ++jt) {
      std::cerr << *jt << ' ';
    } 
    std::cerr << std::endl << std::flush;
    assert(false);
  }
  */
  float deviation = std::abs(info.value + new_value*gamma);
  info.value = -new_value*gamma;
  return deviation;
}


int main() {
  std::unordered_map<StateIndex, StateInfo> states;
  load_data(states);

  const float gamma = 0.99; // discount factor
  const float tol = 1e-5; // tolerance level for conversion criterion
  bool converged = false;
  for (size_t iter=1; iter <= 10000; iter++) {
    std::cout << "Start iteration " << iter << std::endl;
    
    size_t n=0;
    float max_deviation = 0;
    for (std::unordered_map<StateIndex, StateInfo>::iterator it=states.begin(); it != states.end(); ++it) {
      n++;
      float tmp = update_value(states, it->second, gamma);
      if (tmp > max_deviation) max_deviation = tmp;

      // progress report
      if (n % 1000000 == 0) {
        std::cout << "\rUpdating " << n << "/" << states.size() << " (Max deviation so far = " << max_deviation << ")";
      }
    }
    std::cout << std::endl << "End iteration " << iter << " (max deviation = " << max_deviation << ")" << std::endl;
    if (max_deviation < tol) {
      std::cout << std::endl << "CONVERGED!" << std::endl;
      converged = true;
      break;
    }
  }

  if (!converged) {
    std::cout << std::endl << "Did not converge" << std::endl;
    return 1;
  }

  std::cout << "Value of the initial state: " << get_value(states, 2276617967770) << std::endl;
  std::cout << "and its continuation states: " 
    << get_value(states, 2276617162570) << ' '
    << get_value(states, 2276617484980) << ' '
    << get_value(states, 2276617961170) << ' '
    << get_value(states, 87014145179536) << std::endl;

  // write the result to the output file
  std::cout << "Writing the result to the file" << std::endl;
  std::ofstream outfile("doubutsu-values-full.txt");
  size_t n = 0;
  for (std::unordered_map<StateIndex, StateInfo>::iterator it=states.begin(); it != states.end(); ++it) {
    n++;
    // progress report
    if (n % 1000000 == 0 || n == states.size()) {
      std::cout << "\rWriting " << n << "/" << states.size();
    }
    outfile << (it->first) << ' ' << (it->second).value << std::endl;
  }
  outfile.close();
  std::cout << std::endl << "Finished" << std::endl;
  return 0;
}