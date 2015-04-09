front-end
=========

This module is responsible for the lexical and syntactican analysis of the source code.
The output of the front-end operation is an AST structure - a tree or a list of trees.
The symbolic expressions based syntax is supported.

                   +-----------+
    source code -> | front-end | -> AST
                   +-----------+

The sub-modules are:
 * **tokenizer**        converts stream of characters into a stream of tokens
 * **lexer**            builds a symbolic expression DOM tree from the tokens stream
 * **dom analyzer**     manages the DOM tree structure creation and analysis
 * **parser**           parses dom trees or subtrees into AST structures

See also
--------
 * ast module

