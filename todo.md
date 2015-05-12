TODO
----
 * [ ] Improve the packing of the arrays storage not to contain a header
    - fix the code around the occurances of the rt_val_push_array_init
      to properly pack the data inside arrays; disallow compound values in arrays :(
    - fix the code that accesses arrays sequentially (e.g. rt_val_cpd_next_loc)
    - add bool argument to eval_impl that enables prevengint pusing value header.
      Use it in the compound evaluation and compount BIFs
 * [ ] Add benchmarking facilities to support possible optimization refactoirng
 * [ ] Add "random" BIFs at least for uniform number, optionally for exp and normal
 * [ ] Add compound construction BIFs, e.g. range: (func (from to) ...)
 * [ ] Add "try" parafunction?
 * [ ] Implement API for client BIFs, let's call hem CLIFs standing for CLient Implemented Functions
