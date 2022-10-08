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


void read_test() {
  std::set<StateIndex> all_indices;
  std::set<StateIndex> all_target_indices;

  std::ifstream infile("testdata.txt");
  std::string line;
  size_t n_lines = 0;
  while ( std::getline(infile, line) ) {
    std::cout << line << std::endl;
    n_lines++;
    //if (n_lines % 1000000 == 0) std::cerr << "\rReading line " << n_lines;
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

  std::cout << "Number of items read " << all_indices.size() << std::endl;
  for (std::set<StateIndex>::iterator it=all_indices.begin(); it != all_indices.end(); ++it) {
    std::cout << *it << std::endl;
  }
}


int main() {
  read_test();
  return 0;
}