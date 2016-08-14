# TODO
* Change sizes encoding scheme. No longer encode sizes along each value but:
   * for regular values store size when performing bind
   * for array elements - encode element size into array value
   * for tuples encode the sequence of the element sizes into the tuple value
* A bind should create "references", i.e. a tuples of address and type.
  This should be a common concept also used by the reference varaibles. Once
  the references are implemented in terms of address+size the metadata may start
  being removed from the stack. There may be optional boxed values for generic
  implementations...
* Move special forms out of the ast
* Match AST structure is bifurcated, thus the search in the evaluate function definition will skip the keys list
    * all the operations on ast should be based on the centralized generic ast algorithms like find or visit
* Reduce the number of the reference/pointer special forms. Only adress of, peek and poke are essential.
* validate AST structure in the node constructors (add parser tests here)
* parser will now take the ast-loc-map as an argument and fill in if not null
* the mapping between the semantic and terminal symbols such as "UNIT" or "func" must be
  identical in the serialization and the parsing algorithms.
* revise error messages - improve with the new source location tracking mechanism
    * Are the source locations available wherever possible?
* add another debug map: address -> source location for runtime values
* store dom node begin and node end location in the dom nodes
* revise parser and runtime tests
* Consider type requirements in the pattern. How to say (bind (type-constraint x (array-of Integer)))?
* If type expressions are present, Implement function overloading so that more than one function is allowed
  to be bound to a single symbol and it is not an error to fail binding to the formal arguments if there
  is yet another overload to be considered
* If overloading implemented test different matches
* Serialize functions on the stack and enable execution
* Remove automatic capture from lambdas and require user to bind needed values explicitly.
