# Baleen #
Baleen is a library for optimizing [bril](https://capra.cs.cornell.edu/bril/intro.html) programs. It is an implementation of the concepts covered by Cornell's free online compilers course [cs6120](https://www.cs.cornell.edu/courses/cs6120/2020fa/self-guided/). Work is currently paused because I moved on to optimizing LLVM IR programs.

## Features ##
- [x] serialize and deserialize bril programs
- [x] dead code elimination
- [x] local value numebering
    - [x] copy propogation
    - [x] constant propogation
- [x] data flow analysis 
    - [x] "reaching" analysis
- [ ] global analysis and optimization
- [ ] and more...

## Building ##
Baleen uses cmake. I think incorporating Baleen into your own codebase should be as simple as cloning the source tree and calling add_subdirectory(). See tests/test_runner.cpp for some basic usage.
```
mkdir build
cmake -B build
cd build
make
```

## Brilopt ##
A command line utility that takes bril on stdin and prints optimized bril to stdout
```
./build/brilopt < tests/bril/eight-queens.json > optimized.json
```

## Testing ##
Baleen uses ctest. There are a variety of bril programs in test/bril/ which are automatically run and I check that their output is the same before and after optimization. 
- this is not a very robust test suite
- this was my first time writing compiler optimizations so you shouldn't take their quality too seriously
```
cd build
make test
```

