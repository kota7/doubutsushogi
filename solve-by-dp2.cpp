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

void load_data(std::unordered_map<StateIndex, StateInfo> &states, std::ofstream &outfile) {
  std::ifstream infile("doubutsu-all-states.txt");
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
    // we skip the states with win=1 because they won't be updated.
    // write this to the output file
    if (win) {
      outfile << idx << ' ' << 1 << std::endl;
      continue;
    }

    StateInfo info;
    info.value = 0;
    info.next_state_indices.resize(k);
    for (size_t i=0; i<k; i++) stream >> info.next_state_indices[i];

    states[idx] = info;
  }
  infile.close();
  // the number of states is smaller than the number of lines because we omit the states with win=1
  std::cerr << std::endl << n_lines << " lines read, " << states.size() << " states remaining" << std::endl;
}

float get_value(const std::unordered_map<StateIndex, StateInfo> &states, StateIndex idx) {
  auto it = states.find(idx);
  // if this state is not in the info, this is a winning state.
  if (it == states.end()) return 1;
  return (it->second).value;
}

float update_value(std::unordered_map<StateIndex, StateInfo> &states, StateInfo &info, float gamma) {

  // Find the minimum continuation value (least beneficial to the opponent)
  // Then, its negative (times discount factor) becomes the new value for this index
  float new_value = 999;
  for (std::vector<StateIndex>::iterator jt=info.next_state_indices.begin(); jt != info.next_state_indices.end(); ++jt) {
    float tmp = get_value(states, *jt);
    if (tmp < new_value) new_value = tmp;
  }
  float deviation = std::abs(info.value + new_value*gamma);
  info.value = -new_value*gamma;
  return deviation;
}


int main() {
  std::unordered_map<StateIndex, StateInfo> states;
  std::ofstream outfile("doubutsu-values-1b.txt");
  load_data(states, outfile);
  outfile.close();

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
  std::ofstream outfile2("doubutsu-values-2b.txt");
  size_t n = 0;
  for (std::unordered_map<StateIndex, StateInfo>::iterator it=states.begin(); it != states.end(); ++it) {
    n++;
    // progress report
    if (n % 1000000 == 0 || n == states.size()) {
      std::cout << "\rWriting " << n << "/" << states.size();
    }
    outfile2 << (it->first) << ' ' << (it->second).value << std::endl;
  }
  outfile2.close();
  std::cout << std::endl << "Finished" << std::endl;
  return 0;
}