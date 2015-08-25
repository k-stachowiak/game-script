TODO
----
- Rework the pattern module (header already in place; implementation
- allow type checking in the pattern matching
  - add typed matching bind tests to e.g. "(bind (! [char] [ 'a' x ]))"
  - Add typed variant of the pattern AST node
  - Incorporate new symbol (e.g. "?" so "[?]" means any array) for "match anything" (this is different
    from "don't care" in the way that DC will only match one value and "any"
        is more like regex ".+"
            - add overloading
