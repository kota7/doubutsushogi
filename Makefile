generate-all-states: doubutsushogi.o generate-all-states.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o generate-all-states generate-all-states.cpp doubutsushogi.o

generate-all-states2: doubutsushogi.o generate-all-states2.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o generate-all-states2 generate-all-states2.cpp doubutsushogi.o

solve-by-dp: solve-by-dp.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o solve-by-dp solve-by-dp.cpp

solve-by-dp2: solve-by-dp2.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o solve-by-dp2 solve-by-dp2.cpp

solve-by-dp3: solve-by-dp3.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o solve-by-dp3 solve-by-dp3.cpp

validate-data: validate-data.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o validate-data validate-data.cpp

test: doubutsushogi.o test.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o test test.cpp doubutsushogi.o

read-test: read-test.cpp
	g++ -std=c++11 -Wall -Werror -O3 -o read-test read-test.cpp

doubutsushogi.o : doubutsushogi.cpp doubutsushogi.h
	g++ -std=c++11 -Wall -Werror -O3 -c doubutsushogi.cpp doubutsushogi.h

all: generate-all-states generate-all-states2 test solve-by-dp solve-by-dp2 solve-by-dp3 validate-data read-test

clean:; rm *.o
