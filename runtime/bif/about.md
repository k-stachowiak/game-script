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
 * sqrt     : _numeric_ -> _numeric_
 * +        : _numeric_ -> _numeric_ -> _numeric_
 * -        : _numeric_ -> _numeric_ -> _numeric_
 * *        : _numeric_ -> _numeric_ -> _numeric_
 * /        : _numeric_ -> _numeric_ -> _numeric_
 * %        : _numeric_ -> _numeric_ -> _numeric_
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
 * =    : _?_ -> _?_ -> _boolean_
 * <    : _numeric_ -> _numeric_ -> _boolean_
 * >    : _numeric_ -> _numeric_ -> _boolean_
 * <=   : _numeric_ -> _numeric_ -> _boolean_
 * >=   : _numeric_ -> _numeric_ -> _boolean_

**Note**
The equality function is defined for any value.
This function realizes a member-wise comparison of values of any possible complexity, which is trivial since the semantics of the language support contiguous sorage and therefore bitwise comparison will do as the implementation of the _=_ function.

Logic (boolean) functions
-------------------------
 * &&   : ... -> _boolean_ -> _boolean_ -> _boolean_
 * ||   : ... -> _boolean_ -> _boolean_ -> _boolean_
 * ^^   : _boolean_ -> _boolean_ -> _boolean_
 * ~~   : _boolean_ -> _boolean_ -> _boolean_

**Note**
The _&&_ and _||_ functions handle so called short circuit evaluation; e.g. if any argument of the _&&_ turns out false no further arguments are evaluated.
Additionally they will accept an arbitrary number of arguments.

Array functions
---------------
 * length  : _compound_ -> _integer_
 * at      : _compound_ -> _integer_ -> _?_
 * reverse : _compound_ -> _compound_
 * cat     : _compound_ -> _compound_ -> _compound_
 * slice   : _compound_ -> _integer_ -> _integer_ -> _array_

**Note**
If more than one argument is of a compound type it is expected for the arguments to be of the same types.
The _cat_ function which produces a compound value will perform a homogenity check if an array is to be returned.

Text functions
--------------
 * print        : _string_ -> _void_
 * format       : _string_ -> _tuple_ -> _string_
 * to-string    : _?_ -> _string_
 * parse        : _string_ -> { _boolean_ _?_ }
 * parse-bool   : _string_ -> { _boolean_ _boolean_ }
 * parse-char   : _string_ -> { _boolean_ _character_ }
 * parse-int    : _string_ -> { _boolean_ _integer_ }
 * parse-real   : _string_ -> { _boolean_ _real_ }

