Para-functions
==============

This document presents the so-called parafunctions.
They differ from the regular functions in the way they are evaluated.
In case of the regular functions all the actual argments are evaluated before the function body.
This however disables implementation of certain constructs such as short-circuiting "and" operators which will only evaluate enough operands to determine the outcome.
This allows us to control the execution of such expression so that its arguments do not always have to be valid and evaluable in the given context.
There are several types of parafunctions available in the language.

Short-cirquit logical operators
-------------------------------
* (and x1 x2 ...) : Returns the conjunction of arguments
* (or x1 x2 ...) : Returns the disjunction of arguments

Control flow expressions
------------------------
* (if test t f) : Returns t expression if test returns true, and f expression otherwise.
* (while test expr) : Repeatedly executes the expr expression while the test returns true.
* (switch test { x1 v1 } { x2 v2 } ...) : compares test expression against x1, x2, etc. until equality is satisfied.
                                          Returns the "v" value for the matched "x" value.

Reference related expressions
-----------------------------
* (ref value) : Returns reference to a value
* (peek reference) : Returns the value pointed by the reference
* (poke reference value) : Sets a new value to the refered location. Types (and sizes) must match for this to succeed.
* (begin compound) : Returns a reference to the first element of a compound value.
* (end compound) : Returns a reference to the one after last element of a compound value.
* (succ reference) : Returns the successor of the given reference.