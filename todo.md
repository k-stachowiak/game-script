TODO
----
 * [ ] Improve the packing of the arrays storage not to contain a header
    - stop tracking size in all values
    - assure only rt_val API is used for the compound access (i.e. no raw pushes outside rt_val module)
 * [ ] Add benchmarking facilities to support possible optimization refactoirng
 * [ ] Add "random" BIFs at least for uniform number, optionally for exp and normal
 * [ ] Add compound construction BIFs, e.g. range: (func (from to) ...)
 * [ ] Add "try" parafunction?
