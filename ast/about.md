ast
===

This module defines the Abstract Syntax Tree structures.
They reflect the actual semantic structure of the source code and they are what the interpreter processes.
Most of the AST structures are of one of the following types:
 * atomic symbolic expression representing a literal, a reference, etc.,
 * list symbolic expression starting from a keyword, e.g. "do" block: (do ...).

There is, however, a special type of the AST nodes, which is a pattern.
A Pattern is a symbolic expression which is one of the following:
 * atomic symbol,
 * an array of patterns ("[pattern1 pattern2 ...]"),
 * a tuple of patterns (analogous to array of patterns).

The details about the regular AST nodes and the pattern nodes may be found in the respective C source files here.
