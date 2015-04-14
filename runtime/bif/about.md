BIF
===

This document lists the Built-In Functions provided by the language.
They are supposed to be basic operations within the computational basis of the built-in types.

**Note**
 - _?_          ::= any type
 - _numeric_    ::= _integer_ or _real_
 - _compound_   ::= _array_ or _tuple_

Also note that some functions return the error code alongside the result as a tuple.
Although many functions may fail in the runtime (e.g. accessing array element beyond bounds) only some of them handle error codes.
A rule is followed that the error codes are only returned by the functions of which the correctness cannot be verified beforehand.
For example client cannot verify whether a string can be parsed to an integer otherwise than trying the call to _parse-int_.
However client can verify whether access of the i-th element of an array is legal by checking the array's lenght up front.

Arythmetic functions
--------------------
 * +        : _numeric_ -> _numeric_ -> _numeric_
 * -        : _numeric_ -> _numeric_ -> _numeric_
 * *        : _numeric_ -> _numeric_ -> _numeric_
 * /        : _numeric_ -> _numeric_ -> _numeric_
 * %        : _numeric_ -> _numeric_ -> _numeric_
 * sqrt     : _numeric_ -> _numeric_
 * floor    : _real_ -> _integer_
 * ceil     : _real_ -> _integer_
 * round    : _real_ -> _integer_

**Note**
In case of binary arythmetic operation, if type of one argument is _real_ and the type of the other one is _integer_ apromotion takes place.
The integer operand is converted to real type and the operation is performed on the real values.
The returned value is always of _real_ type.

In order to make sure that the result of an arythmetic computation is of integral type, one of the narrowing operations must be used such as _round_.

Comparison functions
--------------------
 * eq   : _?_ -> _?_ -> _boolean_
 * lt   : _numeric_ -> _numeric_ -> _boolean_

**Note**

The equality function is defined for any value.
This function realizes a member-wise comparison of values of any possible complexity, which is trivial since the semantics of the language support contiguous sorage and therefore bitwise comparison will do as the implementation of the _=_ function.

Also note that only the less than operator has been provided as a BIF since the remaining ones such as greater than, etc. may be implemented in terms of the equality and the less than operator.

Logic (boolean) functions
-------------------------
 * and  : ... -> _boolean_ -> _boolean_ -> _boolean_
 * or   : ... -> _boolean_ -> _boolean_ -> _boolean_
 * xor  : _boolean_ -> _boolean_ -> _boolean_
 * not  : _boolean_ -> _boolean_ -> _boolean_

**Note**
The _&&_ and _||_ functions handle so called short circuit evaluation; e.g. if any argument of the _&&_ turns out false no further arguments are evaluated.
Additionally they will accept an arbitrary number of arguments.

Compound functions
------------------
 * push-front   : _compound_ -> _?_ -> _compound_
 * push-back    : _compound_ -> _?_ -> _compound_
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

Text functions
--------------
 * print        : _string_ -> _void_
 * format       : _string_ -> _tuple_ -> _string_
 * to\_string    : _?_ -> _string_
 * parse        : _string_ -> { _boolean_ _?_ }
 * parse\_bool   : _string_ -> { _boolean_ _boolean_ }
 * parse\_char   : _string_ -> { _boolean_ _character_ }
 * parse\_int    : _string_ -> { _boolean_ _integer_ }
 * parse\_real   : _string_ -> { _boolean_ _real_ }

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
