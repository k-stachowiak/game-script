TODO
----
	- debug the crash upon generating an array of 500 random elements
	 	- investigate the reason for which the symbol map in a recursive call has many parents; is this necessary?
    - make function call parser accept any expression in the first position and in the runtime check if it evaluates to function object
	- consider extracting the function arguments evaluation before the decision ast/bif/clif once all tests pass
	- the runtime register of the bound symbols should be realized in terms of a global symbol map
	- implement an std function that would realize bind functionality with placeholders
