Solve Doubutsu Shogi Game
=========================

We find the perfect solution to the [doubutsu shogi game](https://en.wikipedia.org/wiki/D%C5%8Dbutsu_sh%C5%8Dgi).

Note that this solution is not new. A solution to this game has already been presented by Tanaka in [this paper](https://www.tanaka.ecc.u-tokyo.ac.jp/ktanaka/dobutsushogi/animal-private.pdf) and [his program](https://www.tanaka.ecc.u-tokyo.ac.jp/ktanaka/dobutsushogi/).

This program achieves the same goal with slightly different implementation. 
A difference from the original work is that we find values to states on off-optimal paths.


## Steps


### Compile C++ programs

The main solvers are written in C++, which we can be compiled with the following command:

```shell
make all
```

### Generate state list

The following command generates all legal state indices and save to a text file:


```shell
./generate-all-states2
```

Output files:
- `doubutsu-states-full.txt`: list of all legal state indices
- `doubutsu-states-win.txt`: list of all states where the mover can win in one move

### Solve the game

The following command solves the game:

```shell
./solve-by-dp3
```

Input file:
- `doubutsu-states-full.txt`

Output file:
- `doubutsu-values-full.txt`: all state indices and the state values

### Make the SQLite database

```shell
# requires the tqdm library
python3 make-sqlite-db.py
```

Input file:
- `doubutsu-values-full.txt`: all state indices and the state values

Output file:
- `doubutsu.db`: SQLite file
- `doubutsu.db.bz2`: BZ2 compressed (for the release)


## Technical notes

### State representations

We represent a state by a 18-vector of short integers.
The first 12 represents the board cell values in the row-first order.
The cell values are represented by the following constants and are negated for the second mover pieces.

```
EMPTY = 0
HIYOKO = 1
ZOU = 2
KIRIN = 3
LION = 4
NIWATORI = 5
```

The next 3 are the number of captured pieces by the first player (`HIYOKO`, `ZOU`, `KIRIN`) and the last 3 are those of the second player.

### State indices

We define the index of the states by the following formula:

$$
\mathrm{StateIndex}(s) = \sum_{i=1}^{12} 11^{i-1} (s_i + 5) + \sum_{i=13}^{18} 11^{12} \times 3^{i-13} s_i
$$

and its C++ implementation is as follows:
```cpp
unsigned long long state_index(const State &s) {
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
```

### Equivalent states

We eliminate the equivalent states from the list to reduce the computational costs.

- We assume the next mover is always the first player (i.e. lion at the bottom). The states with the second player's turn can still be evaluated by flipping the board and negating the value. 
- We eliminate states one of the holizontally symmetrical board posititions. We only keep the smaller index.
