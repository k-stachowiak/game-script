TODO
----
    - allow type checking in the pattern matching
        - Add typed variant of the pattern AST node
        - Incorporate new symbol (e.g. "?") for "match anything" (this is different
          from "don't care" in the way that DC will only match one value and "any"
          is more like regex ".+"
    - add overloading
