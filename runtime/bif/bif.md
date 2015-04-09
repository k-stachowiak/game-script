BIF
===

This document lists the Built-In Functions provided by the language.
They are supposed to be basic operations within the computational basis of the built-in types.

**Note**
 - _?_ ::= any type
 - _numeric_ ::= _integer_ or _real_

Arythmetic functions
--------------------
 * sqrt  : _numeric_ -> _numeric_
 * +     : _numeric_ -> _numeric_ -> _numeric_
 * -     : _numeric_ -> _numeric_ -> _numeric_
 * *     : _numeric_ -> _numeric_ -> _numeric_
 * /     : _numeric_ -> _numeric_ -> _numeric_
 * %     : _numeric_ -> _numeric_ -> _numeric_
 * floor : _real_ -> _integer_
 * ceil  : _real_ -> _integer_
 * round : _real_ -> _integer_

**Note**
In case of binary arythmetic operation, if type of one argument is _real_ and the type of the other one is _integer_ apromotion takes place.
The integer operand is converted to real type and the operation is performed on the real values.
The returned value is always of _real_ type.

In order to make sure that the result of an arythmetic computation is of integral type, one of the narrowing operations must be used such as _round_.

Array functions
---------------
 * length  : _array_ -> _integer_
 * empty   : _array_ -> _boolean_
 * at      : _array_ -> _?_ **implement**
 * car     : _array_ -> _?_ **remove**
 * cdr     : _array_ -> _array_ **remove**
 * reverse : _array_ -> _array_
 * cons    : _?_ -> _array_ -> _array_ **remove**
 * cat     : _array_ -> _array_ -> _array_
 * slice   : _array_ -> _integer_ -> _integer_ -> _array_

Comparison functions
--------------------
 * == : _numeric_ -> _numeric_ -> _boolean_
 * <  : _numeric_ -> _numeric_ -> _boolean_
 * >  : _numeric_ -> _numeric_ -> _boolean_
 * <= : _numeric_ -> _numeric_ -> _boolean_
 * >= : _numeric_ -> _numeric_ -> _boolean_

Logic (boolean) functions
-------------------------
 * && : _boolean_ -> _boolean_ -> _boolean_
 * || : _boolean_ -> _boolean_ -> _boolean_
 * ^^ : _boolean_ -> _boolean_ -> _boolean_
 * ~~ : _boolean_ -> _boolean_ -> _boolean_

 **Note**
 The _&&_ and _||_ functions handle so called short circuit evaluation; e.g. if any argument of the _&&_ turns out false no further arguments are evaluated.

Text functions
--------------
 * putc         : _character_ -> _void_
 * print        : _string_ -> _void_
 * format       : _string_ -> _tuple_ -> _string_
 * to-string    : _?_ -> _string_
 * parse        : _string_ -> { _boolean_ _?_ }
 * parse-bool   : _string_ -> { _boolean_ _boolean_ }
 * parse-char   : _string_ -> { _boolean_ _character_ }
 * parse-int    : _string_ -> { _boolean_ _integer_ }
 * parse-real   : _string_ -> { _boolean_ _real_ }
