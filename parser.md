front-end
=========

This module is responsible for the lexical and syntactican analysis of the source code.
The output of the front-end operation is an AST structure - a tree or a list of trees.
The symbolic expressions based syntax is supported.

                   +-----------+
    source code -> | front-end | -> AST
                   +-----------+

*NOTE* Several elements depend on the context, especially the pattern construct.
A very similar syntactical structure may have different semantics, e.g.:

    (bind x [alpha beta gamma])

means bind an array of three references to the symbol "x".
Here the bracket enclosed structure means an array literal, whereas

    (bind [alpha beta gamma] x)

means assume x to be a three element array and bind the consecutive values to the three symbols.
In this case the bracket enclosed structure means a pattern to be matched.

The sub-modules are:
 * **tokenizer**        converts stream of characters into a stream of tokens
 * **lexer**            builds a symbolic expression DOM tree from the tokens stream
 * **dom analyzer**     manages the DOM tree structure creation and analysis
 * **parser**           parses dom trees or subtrees into AST structures

See also
--------
 * ast module

