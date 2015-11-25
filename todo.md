TODO
----
- Replace switch parafunc with match full-blown match expression
    - ast\_make\_literal\_string should make copy of input internally! And so should pattern counterpart.
    - eval\_bind\_pattern should be actually called verify match, because it doesn't put anything on stack.
      (or does it?)
- Replace short circuit para functions with "all\_of", "any\_of", etc. algorithms.
- Consider type requirements in the pattern (WHAT SHOULD BE THE SYNTAX FOR THIS???)
- If type expressions are present, Implement function overloading so that more than one function is allowed
  to be bound to a single symbol and it is not an error to fail binding to the formal arguments if there
  is yet another overload to be considered
- Serialize functions on the stack and enable execution
