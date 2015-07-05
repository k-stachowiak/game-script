TODO
----
    - reduce homo check to only pair of objects and implement the loop in the BIF module
    - inscrease constraints on array. Homogenous means equal binary footprint, e.g. arrays of equal lengths
    - make function call parser accept any expression in the first position and in the runtime check if it evaluates to function object
    - add compound functions like cpd\_lt and cpd\_sort (if sorting array, use quicksort, if tuple then use mergesort)
	- consider extracting the function arguments evaluation before the decision ast/bif/clif once all tests pass
    - upon error the debugger indentation counter doesn't reset
	- the runtime register of the bound symbols should be realized in terms of a global symbol map
	- implement an std function that would realize bind functionality with placeholders
