TODO
----
 * [ ] Improve the packing of the arrays storage not to contain a header
    - fix the code around the occurances of the rt_val_push_array_init
      to properly pack the data inside arrays; disallow compound values in arrays :(
    - fix the code that accesses arrays sequentially (e.g. rt_val_cpd_next_loc)
 * [ ] Add benchmarking facilities to support possible optimization refactoirng
 * [ ] Add "random" BIFs at least for uniform number, optionally for exp and normal
 * [ ] Add compound construction BIFs, e.g. range: (func (from to) ...)
 * [ ] Add "try" parafunction?
