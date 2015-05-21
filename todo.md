TODO
----
 * [ ] Analyze std library load case. Why no error?
		- decrease granularity of module info in error structure (e.g. just eval, and not eval bif...)
		- Observe all the error propagaion paths - prevent meaningless duplicates
		- remove the location stack once the error stack works
 * [ ] refactor repl special commands to use CLIF mechanism (another approach to totally move towards lib API).
 * [ ] How is source location propagated upon function call (arguments)?
