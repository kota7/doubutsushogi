#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <cassert>

// We redefine this type here
// to make this script independent of the doubutsushogi.h
using StateIndex = unsigned long long;


void validate_data() {
  std::set<StateIndex> all_indices;
  std::set<StateIndex> all_target_indices;

  std::ifstream infile("doubutsu-all-states.txt");
  std::string line;
  size_t n_lines = 0;
  while ( std::getline(infile, line) ) {
    n_lines++;
    if (n_lines % 1000000 == 0) std::cerr << "\rReading line " << n_lines;
    std::istringstream stream(line);

    StateIndex idx;
    bool win;
    size_t k;
    stream >> idx >> win >> k;

    if (all_indices.find(idx) != all_indices.end()) {
      std::cerr << std::endl << "Index " << idx << " is duplicated at line " << n_lines << std::endl;
    }
    all_indices.insert(idx);
    if (win==1 && k > 0) {
      std::cerr << std::endl << "Non-unique next indices for a winning state " << idx << " at line " << n_lines << std::endl;
    }
  
    for (size_t i=0; i<k; i++) {
      StateIndex idx2;
      stream >> idx2;
      all_target_indices.insert(idx2);
    }
  }
  infile.close();
  std::cerr << std::endl << n_lines << " lines read, " << all_indices.size() << " unique indices" << std::endl;
  if (n_lines != all_indices.size()) std::cerr << "Index uniqueness violated!" << std::endl;

  size_t j = 0;
  for (std::set<StateIndex>::iterator it=all_target_indices.begin(); it != all_target_indices.end(); ++it) {
    j++;
    if (j % 1000000 == 0 || j == all_target_indices.size()) std::cerr << "\rChecking the target index " << j << "/" << all_target_indices.size();

    if (all_indices.find(*it) == all_indices.end()) {
      std::cerr << std::endl << "Target index " << *it << " not in the indices." << std::endl;
    }
  }
  std::cerr << std::endl;
}


int main() {
  validate_data();
  return 0;
}