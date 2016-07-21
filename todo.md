# TODO
- track the source location information propagation in the error reporting.
- revise error messages - improve with the new source location tracking mechanism
    - Are the source locations available wherever possible?
- add another debug map, this time address -> source location for runtime values
- eval_dispach and eval_special may be implemented in term of function table
- store dom node begin and node end location in the dom nodes
- Consider type requirements in the pattern (WHAT SHOULD BE THE SYNTAX FOR THIS???)
- migrate from cmake to make; think of carving out libraries such as a parser, runtime, semantics (AST, etc.), etc.
- A bind should create "references", i.e. a tuples of address and type.
  This should be a common concept also used by the reference varaibles. Once
  the references are implemented in terms of address+size the metadata may start
  being removed from the stack. There may be optional boxed values for generic
  implementations...
- If type expressions are present, Implement function overloading so that more than one function is allowed
  to be bound to a single symbol and it is not an error to fail binding to the formal arguments if there
  is yet another overload to be considered
- If overloading implemented test different matches
- Serialize functions on the stack and enable execution
- Remove automatic capture from lambdas and require user to bind needed values explicitly.
