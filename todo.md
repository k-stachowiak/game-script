# TODO #
- [BUG] error while parsing standard library, example captured in the CCI test
    - add trace debug messages to the entire evaluation call tree
        - add serialization for AST and Pattern modules to be used in the evaluation tracing
- Bring order to AST. It should only consist of:
  - literals (atomic and compound; constitute a type)
  - special forms
  - function calls
- Consider type requirements in the pattern (WHAT SHOULD BE THE SYNTAX FOR THIS???)
- migrate to auto-tools; think of carving out libraries such as a parser, runtime, semantics (AST, etc.), etc.
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
