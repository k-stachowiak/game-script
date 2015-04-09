runtime
=======

This module provides a state for the execution of the scripts.
In general the AST structures are consumed by the runtime in order to produce output or change the internal state.

                           -----> result value
           +---------+    /
    AST -> | runtime | -<
           +---------+    \
                 ^        /
                  \      /
                   ------
                 state change

The main sub-modules are:
 * **runtime**      the main state of the interpreter
 * **stack**        the memory store
 * **symbols map**  symbols map storing references to values and function (values as well)
 * **eval**         the module that implements specific actions for each of the AST types
 * **BIF**          stores thte built-in function implementations

The helper sub-modules are:
 * **dbg**      enables recursive tracking of all evaluation operations
 * **rt val**   enables input and output of the values in the stack store

See also
--------
 * bif documentation
 * ast module

