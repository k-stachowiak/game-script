TODO
----
    - make function call parser accept any expression in the first position and in the runtime check if it evaluates to function object
	- consider extracting the function arguments evaluation before the decision ast/bif/clif once all tests pass
    - upon error the debugger indentation counter doesn't reset
	- the runtime register of the bound symbols should be realized in terms of a global symbol map
	- implement an std function that would realize bind functionality with placeholders
