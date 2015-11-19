TODO
----
- Replace switch parafunc with match full-blown match expression
    - allow literals in patterns
    - update documentation to describe the patern matching done in the switch.
    - remove switch from parafunctions
- Implement function overloading so that more than one function is allowed to be bound to
  a single symbol and it is not an error to fail binding to the formal arguments if there
  is yet another overload to be considered
- Serialize functions on the stack and enable execution
- Consider type requirements in the pattern (WHAT SHOULD BE THE SYNTAX FOR THIS???)
