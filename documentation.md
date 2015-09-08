Overview
========

This is a documentation of the scripting language called MOON. The name is an acronym for

    Multi-paradigm, Object Oriented... Not!

This is a great example of a language consistengs of layers over layers of afterthought.
In fact "afterthought" may have been a better name but it doesn't shorten to a swift acronym likt "moon", also it does not differentiate it from myriads of other language design failures.

Language traits
---------------
* syntactically lisp-based (i.e. virtually no syntax)
* implicitly but strongly typed
* simple data-types (boolean, integer, real, character)
* compound data-types (tuple and array)
* higher order functions obtainable by explicit definition or currying
* closures
* immutability by default
* mutability via references
* iterator-like references

Runtime traits
--------------
* entirely stack based (sic!)
* embeddable (REPL application provided as an example)

Language facilities
===================

In the following sections a notation for function definitions will be used.
There are following placeholders for expressing more than a single type
 - _?_          ::= any type
 - _a_          ::= any type matched in the scope of the definition 
 - _numeric_    ::= _integer_ or _real_
 - _compound_   ::= _array_, _tuple_ or _string_ (string is implicitly treated as an array of characters)

Basic mechanisms
----------------

### Literals
There are following basic literals creating values of corresponding types:
* "true" or "false" for boolean values
* integral literal for integer values
* double literal for real values
* apostrophe delimited character for character literals

There are also following compound literals:
* bracket enclosed list of other values creating an array (homogenity check is performed between the values)
* brace enclosed list of other values creating a tuple
* Quote delimited character sequence automatically evaluating to array of characters

Note that homogenity means the exact same type, which is relatively strict for arrays, meaning that not only the stored type but also the length must match.
It is therefore impossible to create a jagged array in this language.

### Bind
Bind operation pushes a new value on the stack and makes it available for further computation in the given stack frame.

### Function declaration
Func operation creates a new function value.
The value captures all the reachable references in terms of the closure (all mismatched references are to be evaluated at call time).
Since the values have been completely immutable at the earlier design stages, the closure will take all captures by falue (yes, even large buffers).

### Function call
Function call by default evaluates the given function object with the provided parameters.
If the actual parameters count is less than the formal parameters count the expression evaluates to a curried function object with the already applied arguments stored within.

### Function objects
As the previous sections indicate function objects are relatively complex.
They consist of:
* (optional) values captured from the creation context
* (optional) already aplied arguments
* pointer to the function definition

Para-functions
--------------
This chapter presents the so-called parafunctions.
They differ from the regular functions in the way they are evaluated.
In case of the regular functions all the actual argments are evaluated before the function body.
This however disables implementation of certain constructs such as short-circuiting "and" operators which will only evaluate enough operands to determine the outcome.
This allows us to control the execution of such expression so that its arguments do not always have to be valid and evaluable in the given context.
There are several types of parafunctions available in the language.

### Short-circuit logical operators
* and : _boolean_ -> _boolean_ -> ... -> _boolean_
* or  : _boolean_ -> _boolean_ -> ... -> _boolean_

### Control flow expressions
* if     : _boolean_ -> _?_ -> _?_ -> _?_
* while  : _boolean_ -> _?_ -> unit
* switch : _a_ -> { _a_ _b_ } -> { _a_ _b_ } -> ... -> _b_

### Reference related expressions
* ref   : _?_ -> _reference_            -- returns reference to a given value
* peek  : _reference_ -> _?_            -- returns value pointed by the reference
* poke  : _reference_ -> _?_ -> _unit_  -- sets value pointed by the reference
* begin : _compound_ -> _reference_     -- returns reference to first element of compound value
* end   : _compound_ -> _reference_     -- returns reference to "one behind last"
* inc   : _reference_ -> _unit_         -- increments the location pointed by a reference
* succ  : _reference_ -> _reference_    -- returns the successor of the given reference

BIF
---
For the functions that are impossible, not optimal etc. for the implementation in thelanguage itself the built-in function mechanism has been provided.
The general purpose of the BIFs is two-fold.

Firstly they enable defining the computational basis for the implementation of the more complex algorithms.
For example for the objects comparison the equality and less-than functions are provided in terms of BIFs which in turn enables implementing the remaining functions (such as greater or equal) in terms of the BIFs using the higher level scripting language.

Secondly in case of embedding the interpreter in an external program the BIFs enable communication between the script and the calling program.
For example the REPL program provides the "quit" function enabling the end-user terminating the program operation by calling a BIF which is syntactically equivalent to calling a regular function.

This chapter lists the Built-In Functions provided by default by the language runtime.

They are supposed to be basic operations within the computational basis of the built-in types.

Also note that some functions return the error code alongside the result as a tuple.
Although many functions may fail in the runtime (e.g. accessing array element beyond bounds) only some of them handle error codes.
A rule is followed that the error codes are only returned by the functions of which the correctness cannot be verified beforehand.
For example client cannot verify whether a string can be parsed to an integer otherwise than trying the call to _parse-int_.
However client can verify whether access of the i-th element of an array is legal by checking the array's lenght up front.

### Arythmetic functions
 * +        : _numeric_ -> _numeric_ -> _numeric_
 * -        : _numeric_ -> _numeric_ -> _numeric_
 * *        : _numeric_ -> _numeric_ -> _numeric_
 * /        : _numeric_ -> _numeric_ -> _numeric_
 * %        : _numeric_ -> _numeric_ -> _numeric_
 * sqrt     : _numeric_ -> _numeric_
 * real     : _integer_ -> _real_
 * floor    : _real_ -> _integer_
 * ceil     : _real_ -> _integer_
 * round    : _real_ -> _integer_

**Note**
In case of binary arythmetic operation, if type of one argument is _real_ and the type of the other one is _integer_ apromotion takes place.
The integer operand is converted to real type and the operation is performed on the real values.
The returned value is always of _real_ type.

In order to make sure that the result of an arythmetic computation is of integral type, one of the narrowing operations must be used such as _round_.

### Comparison functions
 * eq   : _?_ -> _?_ -> _boolean_
 * lt   : _numeric_ -> _numeric_ -> _boolean_

**Note**
The equality function is defined for any value.
This function realizes a member-wise comparison of values of any possible complexity, which is trivial since the semantics of the language support contiguous sorage and therefore bitwise comparison will do as the implementation of the _=_ function.

Also note that only the less than operator has been provided as a BIF since the remaining ones such as greater than, etc. may be implemented in terms of the equality and the less than operator.

### Logic (boolean) functions
 * xor  : _boolean_ -> _boolean_ -> _boolean_
 * not  : _boolean_ -> _boolean_ -> _boolean_

### Compound functions
 * push\_front  : _compound_ -> _?_ -> _compound_
 * push\_back   : _compound_ -> _?_ -> _compound_
 * cat          : _compound_ -> _compound_ -> _compound_
 * length       : _compound_ -> _integer_
 * at           : _compound_ -> _integer_ -> _?_
 * slice        : _compound_ -> _integer_ -> _integer_ -> _array_

**Note**
If more than one argument is of a compound type it is expected for the arguments to be of the same types.
The _cat_ function which produces a compound value will perform a homogenity check if an array is to be returned.

The indices in the _slice_ function refer to the _begining_ of the compound value's cell, so the following are true:

    (= (slice [ 1 2 3 ] 0 0) [])
    (= (slice [ 1 2 3 ] 0 1) [ 1 ])
    (= (slice [ 1 2 3 ] 2 3) [ 3 ])
    (= (slice [ 1 2 3 ] 1 3) [ 2 3 ])
    # etc...

### Text functions
 * print        : _string_ -> _unit_
 * format       : _string_ -> _tuple_ -> _string_
 * to\_string   : _?_ -> _string_
 * parse        : _string_ -> { _boolean_ _?_ }
 * parse\_bool  : _string_ -> { _boolean_ _boolean_ }
 * parse\_char  : _string_ -> { _boolean_ _character_ }
 * parse\_int   : _string_ -> { _boolean_ _integer_ }
 * parse\_real  : _string_ -> { _boolean_ _real_ }

**Note**

_format_ function addepts a *format string* and a tuple of arguments which will be matched to wildcards present in the *format string*.
The format string is a simplified version of the concept present in the C language, in the functions like _sprintf_.
No fine tuning of the format is possible, only the selection of the value's type.
The following wildcards are supported (with the respective matched types):
 * _%b_ : boolean
 * _%c_ : character
 * _%d_ : integer
 * _%f_ : real
 * _%s_ : string

_print_ function will print a given string to the output.
The function is implemented in terms of the C language's _printf_ function and therefore the according escape sequences will be accepted by this language's _print_ function.

### Random number generation functions
 * rand\_ui    : _integer_ -> _integer_ -> _integer_
 * rand\_ur    : _real_ -> _real_ -> _real_
 * rand\_ber   : _real_ -> _boolean_
 * rand\_exp   : _real_ -> _real_
 * rand\_gauss : _real_ -> _real_ -> _real_
 * rand\_distr : [ _real_ ] -> _integer_

These functions are implemented in terms of the C++ standard random library (since C++11).
_ui_ stands for "uniform integer", _ur_ stands for "uniform real" and the rest should be self-explanatory.

### Type inference functions
 * is\_bool         : _?_ -> _boolean_
 * is\_int          : _?_ -> _boolean_
 * is\_real         : _?_ -> _boolean_
 * is\_char         : _?_ -> _boolean_
 * is\_array        : _?_ -> _boolean_
 * is\_tuple        : _?_ -> _boolean_
 * is\_function     : _?_ -> _boolean_
 * is\_reference    : _?_ -> _boolean_

Implementation details
======================

AST
---
The Abstract Syntax Tree structures reflect the actual semantic structure of the source code and they are what the interpreter processes.
Most of the AST structures are of one of the following types:
 * atomic symbolic expression representing a literal, a reference, etc.,
 * list symbolic expression starting from a keyword, e.g. "do" block: (do ...).

There is, however, a special type of the AST nodes, which is a pattern.
A Pattern is a symbolic expression which is one of the following:
 - atomic symbol,
 - an array of patterns ("[pattern1 pattern2 ...]"),
 - a tuple of patterns (analogous to array of patterns).

Front-end
---------
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

Runtime
-------
This module provides a state for the execution of the scripts.
In general the AST structures are consumed by the runtime in order to produce output or change the internal state.

                              -----> result value
              +---------+    /
    AST ----> | runtime | -<
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
 * **rt\_val**   enables input and output of the values in the stack store
