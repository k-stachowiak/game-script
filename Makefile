CXX=g++
CXX_FLAGS=-g -O0 -Wall -Werror -std=c++11 
TEST_LD_FLAGS=-lUnitTest++

.PHONY: test clean doc

test: tokenize_test dom_build_test ast_build_test
	./tokenize_test
	./dom_build_test
	./ast_build_test

tokenize_test: tokenize_test.cpp tokenize.cpp tok.h
	$(CXX) $(CXX_FLAGS) -o tokenize_test tokenize.cpp tokenize_test.cpp $(TEST_LD_FLAGS)

dom_build_test: dom_build_test.cpp dom_build.cpp dom_build.h dom.h
	$(CXX) $(CXX_FLAGS) -o dom_build_test dom_build.cpp dom_build_test.cpp $(TEST_LD_FLAGS)

ast_build_test: ast_build_test.cpp ast_build.cpp ast.cpp ast_build.h ast.h
	$(CXX) $(CXX_FLAGS) -o ast_build_test ast.cpp ast_build_test.cpp $(TEST_LD_FLAGS)

clean:
	find . -maxdepth 1 -executable -type f -exec rm {} \;

doc: doc/doc.tex
	$(MAKE) -C doc
