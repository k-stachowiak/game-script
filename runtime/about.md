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
 * **runtime**      _the main state of the interpreter_
 * **stack**        _the memory store_
 * **symbols map**  _symbols map storing references to values and function (values as well)_
 * **eval**         _the module that implements specific actions for each of the AST types_
 * **BIF**          _stores thte built-in function implementations_

The helper sub-modules are:
 * **dbg**      _enables recursive tracking of all evaluation operations_
 * **rt val**   _enables input and output of the values in the stack store_

See also
--------
 * ast module

