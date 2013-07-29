CXX=g++
CXX_FLAGS=-g -O0 -Wall -Werror -std=c++11 
TEST_LD_FLAGS=-lUnitTest++

.PHONY: test clean

test: tok_test dom_build_test dom_expect_test
	./tok_test
	./dom_build_test
	./dom_expect_test

tok_test: tok_test.cpp tok.cpp tok.h
	$(CXX) $(CXX_FLAGS) -o tok_test tok.cpp tok_test.cpp $(TEST_LD_FLAGS)

dom_build_test: dom_build_test.cpp dom_build.cpp dom_build.h dom.h
	$(CXX) $(CXX_FLAGS) -o dom_build_test dom_build.cpp dom_build_test.cpp $(TEST_LD_FLAGS)

dom_expect_test: dom_expect_test.cpp dom_expect.h dom.h
	$(CXX) $(CXX_FLAGS) -o dom_expect_test dom_expect_test.cpp $(TEST_LD_FLAGS)

clean:
	find . -maxdepth 1 -executable -type f -exec rm {} \;
