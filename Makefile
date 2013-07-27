CXX=g++
CXX_FLAGS=-g -O0 -Wall -Werror -std=c++11 
TEST_LD_FLAGS=-lUnitTest++

.PHONY: test clean

test: tok_test dom_test
	./tok_test
	./dom_test

tok_test: tok_test.cpp tok.cpp tok.h
	$(CXX) $(CXX_FLAGS) -o tok_test tok.cpp tok_test.cpp $(TEST_LD_FLAGS)

dom_test: dom_test.cpp dom.cpp dom.h
	$(CXX) $(CXX_FLAGS) -o dom_test dom.cpp dom_test.cpp $(TEST_LD_FLAGS)

clean:
	find . -maxdepth 1 -executable -type f -exec rm {} \;
