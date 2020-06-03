CCFLAGS=--std=c++11 -pedantic -Wall -Werror -ggdb3 -pthread

# TARGETS= main client_test my_test
TARGETS= scalaMain

all: $(TARGETS)
clean:
	rm -f $(TARGETS)
# client_test: client_test.cpp
# 	g++ -g -o $@ $(CCFLAGS) $?
# main: main.cpp
# 	g++ -g -o $@ $(CCFLAGS) $?
# my_test: clienttest.cpp
# 	g++ -g -o $@ $(CCFLAGS) $?
scalaMain: scalaMain.cpp
	g++ -g -o $@ $(CCFLAGS) $?
