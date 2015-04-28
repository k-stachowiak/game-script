TODO
----
 * [ ] Refactor "rt_val" to a front-end library; update clients to only use that API.
    - remove file parsing from the parsing module (only parse string)
    - runtime consume list function must return the location of the last result
    - lib helpers must provide string and file parsing
    - make REPL only depend on public API
 * [ ] Improve the packing of the arrays storage not to contain a header
 * [ ] Add benchmarking facilities to support possible optimization refactoirng
 * [ ] Add "random" BIFs at least for uniform number, optionally for exp and normal
 * [ ] Add compound construction BIFs, e.g. range: (func (from to) ...)
 * [ ] Add "try" parafunction?
