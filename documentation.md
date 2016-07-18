Overview
========

This is a documentation of the scripting language called MOON. The name is an acronym for

    Multi-paradigm, Object Oriented... Not!

This is a great example of a language consistng of layers over layers of afterthought.
In fact "afterthought" may have been a better name but it doesn't shorten to a swift acronym likt "moon", also it does not differentiate it from myriads of other language design failures.
As of the december of 2015 the code for the interpreter has been tagged as the version 1 and is undergoing a heavy refactoring towards a more reasonable language.
The documentation below describes the most recent goals in the general sections (to convey the ideas) and the current state in the specific sections (to enable usage).

Language traits
---------------
* No syntax yet - symbolic expressions are used to write code.
* Is relatively strongly typed.
* Has algebraic types - complex types are built as products (structs), sums (unions), and powers (arrays) of types.
* Has higher order functions - objects that store state explicitly and provide function call semantics.
* Has Data immutability by default.
* Mutability is achievable via references
* References provide pointer semantics, i.e. dereferencing as well as "pointer arithmetic"

Runtime traits
--------------
* Stack based - heap allocation implemented as a library; no garbage collection
* Embeddable (REPL application provided as an example of calling moon code from C code)
* Expandable (REPL also shows how C functions can be injected into the moon environment)

Language facilities
===================

Basic mechanisms
----------------

### Symbol binding

Any moon program constitutes a set of objects bound to according symbols.
This set is traditionally called the global scope as the objects are accessible via the symbols from wherever in the program (globally).
The introduction of new bindings into the scope is perforned with the `bind` expression.
It is an expression that consists of two elements:
* the binding pattern,
* the bound expression.
It evaluates to the value of the bound expression (following the controversial tradition from the C language).
The example binding a simple value to a simple symbol looks like this:

    (bind x 3),

where `x` is the binding pattern (a simple example of one), and `3` is the bound expression.

We do not always work with simple values and the binding pattern is a tool versatile enough to account for that.
Let's say we know that there exists an array of two elements called `v`, and we want to bind the respective elements to new symbols `x` and `y`.
We may perform this with the following binding expression:

    (bind [ x y ] v).

The expression `[ x y ]` is the binding pattern here, whereas `v` is the bound expression.

It is actually not required for every binding to introduce a new symbol as the patterns provide the mechanism of wildcarding.
If for some reason in the above example we only want to bind the second array element we may write:

    (bind [ _ y ] v).

The patterns syntax and semantics is quite complex compared to the rest of the language.
Also the patterns, hovewer similar to other expressions, have a very different semantics.
Even in cases of identical syntactical strucutre, depending on the context, the given expression may be considered a pattern or an expression, and carry a different meaning respectively.
For these reasons a separate section has been devoted to the detailed explanation of the patterns mechanism.

### Functions

#### Function definition

In the previous section a binding of a simple integer value to a symbol has been presented.
Whereas the binding always connects the symbol with a value, there is a special type of values - the function object.
The main differences between the function object and a regular one are:
* it is the only type that may be used in the function call expression,
* it doesn't provide any mutation semantics.

The function objects are created with a `func` expression, e.g. in order to define a function which returns the square of its argument we can write:

    (func (x) (* x x)),

or bound to a symbol:

    (bind square (func (x) (* x x))).

As any other objects, functions can be passed to a function or returned from one.
Normally this mechanism would not be of much use, hence it was not introduced in languages such as Pascal or C, to an

Literals
--------
One way of expressing a value is a literal expression.
There are following basic literals creating values of corresponding types:
* `unit` is a special value of a single-valued type `unit`
* `true` or `false` for boolean values
* integral literal for integer values (including sign)
* double literal for real values (including sign)
* apostrophe delimited character for character literals

There are also following compound expressions instantly creating a value like the literal expressions:
* bracket enclosed list of other values creating an array (homogenity check is performed between the values),
* brace enclosed list of other values creating a tuple,
* quote delimited character sequence automatically evaluating to array of characters

The examples of the compound expressions enumerated above are respectively:

   [ 1.0 2.0 3.0 ]
   { 1.0 2 "three" }
   "A man, a plan, a canal - Panama!"

Note that homogenity means the exact same type, which is relatively strict for arrays, meaning that not only the stored type but also the length must match.
This mean that an array of arrays will be a rectangular structure not allowing for rows of different lengths a.k.a. jagged arrays.

### Functions

#### Function declaration
"func" operation creates a new function value.
The value captures all the reachable references in terms of the closure (all mismatched references are to be evaluated at call time).
Since the values have been completely immutable at the earlier design stages, the closure will take all captures by falue (yes, even large compound values).

#### Function call
Function call evaluates the given function object with the provided parameters.
If the actual parameters count is less than the formal parameters count the expression evaluates to a curried function object with the already applied arguments stored within.
Passing too many arguments results in a runtime error.

#### Function objects
As the previous sections indicate function objects are relatively complex.
They consist of:
* (optional) values captured from the creation context
* (optional) already aplied arguments
* reference to the function definition

### Pattern matching
The act af assigning a value to a symbol is a case of the mechanism called binding.
There are the following cases of binding in moon:
* bind expression,
* formal function argument,
* keys in the match expression.

The binding semantics supports pattern matching mechanism.
For the bind expression this provides a mechanism of the value analysis like in the following example:

    (bind foo (func (v)
        (bind { x y } v)
    ))

The above function takes an argument v, which effectively means that inside the function the symbol `v` will refer to the argument.
The bind expression inside the function performs matching of the `v` value to a tuple of two values: `x` and `y`.
The matching failure will result in a runtime error, whereas upon success the following conditions hold:
* v is a tuple of two elements
* first element of v is accessible via the symbol x,
* second element of v is accessible via the symbol y.

If the `v` symbol is not needed anywhere inside the function, the pattern matching may be performed earlier, in the argument list.
In such case the above function can be rewritten the following way:

    (bind foo' (func ({ x y })
        # ...
    ))

In order to avoid the aforementioned runtime error in the bind expression a `match` expression may be used.
It is an equivalent of Pascal's `case` statememnt or C's `switch`.
The expression is used the following way:

    # ...
    (match v
        { x } 1
        { x y } 2
        { x y z } 3
        _ -1
    )
    # ...

The above example will match the value `v` against 3 specific patterns (one-, two- and three- element tuples), and the "don't care" pattern `\_`.
The patterns are checked one by one and upon the first matching success the expression to the right is evaluated and its result is returned.
If none of the expressions is successfully matched it will still result in a runtime error, however the "don't care" pattern prevents this completely.

In order to make them conveniently usable, patterns are context-dependent symbolic expressions.
This means that for example an expression:

    [ 1 2 3 ]

may mean both:
* literal expression creating a 3-element array,
* pattern which will match a particular array value.

In order to avoid ambiguity the patterns are treated as a separate syntactic cathegory compared to all the other language expressions.
Also they match the rest of the language in terms of complexity which further justifies their separation.

A Pattern is a symbolic expression which is one of the following:
 - a don't care token "\_" (matches any value without binding it to a symbol)
 - atomic symbol (matches any value and binds it to a symbol),
 - an array of patterns (matches an array and binds its elements to the given patterns),
 - a tuple of patterns (analogous to array of patterns but for a tuple).

Para-functions
--------------
The para-functions differ from the regular functions in the way they are evaluated.
In case of the regular functions all the actual argments are evaluated before the function body.
This however disables implementation of certain constructs such as short-circuiting logical operators which only evaluate enough operands to determine the outcome.

The parafunctions are built into the language runtime with custom evaluation procedures implemented to control the execution of such expressions.
Therefore a para-funcion can be valid and evaluable as a whole even if certain sub-expressions are not in a given context.
There are several types of parafunctions available in the language.

### Short-circuit logical operators
* and : _boolean_ -> _boolean_ -> ... -> _boolean_
* or  : _boolean_ -> _boolean_ -> ... -> _boolean_

### Control flow expressions
* if     : _boolean_ -> _?_ -> _?_ -> _?_
* while  : _boolean_ -> _?_ -> unit

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
