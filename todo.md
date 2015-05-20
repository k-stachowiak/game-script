TODO
----
 * [ ] Analyze std library load case. Why no error?
		- Improve the evaluation/error reporting facility to provide entire call stack upon error.
		- Observe all the error propagaion paths - prevent meaningless duplicates
		- remove the location stack once the error stack works
 * [ ] refactor repl special commands to use CLIF mechanism (another approach to totally move towards lib API).
 * [ ] How is source location propagated upon function call (arguments)?
