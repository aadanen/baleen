# Baleen #
Baleen is a library for optimizing [bril](https://capra.cs.cornell.edu/bril/intro.html) programs. It is an implementation of the concepts covered by Cornell's free online compilers course [cs6120](https://www.cs.cornell.edu/courses/cs6120/2020fa/self-guided/)

## Features ##
- [x] serialize and deserialize bril programs
- [ ] dead code elimination
- [ ] local value numebering
- [ ] data flow analysis
- [ ] global analysis and optimization
- [ ] and more...

## Building ##
Baleen uses cmake. I think incorporating Baleen into your own codebase should be as simple as cloning the source tree and calling add_subdirectory(). See tests/test_runner.cpp for some basic usage. As I do more work and the library gets more featureful I will provide more examples.

## Testing ##
```
mkdir build
cmake -B build
cd build
make
make test
```

