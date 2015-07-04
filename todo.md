TODO
----
    - add type analytical BIFs (is_array, is_tuple, etc.)
    - have distinction between range algorithms and value algorithms
    - separate sorting functions into reference sorting and value sorting (value sorting by mergesort)
    - add compound functions like cpd_lt and cpd_sort (if sorting array, use quicksort, if tuple then use mergesort)
	- consider extracting the function arguments evaluation before the decision ast/bif/clif once all tests pass
    - upon error the debugger indentation counter doesn't reset
	- the runtime register of the bound symbols should be realized in terms of a global symbol map
	- implement an std function that would realize bind functionality with placeholders
