# RoboGo

[![Build Status](https://travis-ci.com/Kareem-Emad/RoboGo.svg?token=ensmVBcwHV9ec6ob8Yqq&branch=master)](https://travis-ci.com/Kareem-Emad/RoboGo)

## Formatting

- Before submitting pull requests, make sure that you run clang-format so that
your code follows our standards, otherwise, travis will complain.

- If you already made your commit, you can run `git clang-format HEAD~1` to
format it, or you can use `clang-format <path-to-file> -i` from the beginning.

## Testing

- set up

```shell
g++ -c tests/main.cpp -o tests/main.o
```

*avoid compiling main.cpp again, it takes time*

- to run tests

```shell
g++ tests/main.o tests/*test-file*.cpp -o test-run 
./test-run
rm test-run.exe
```

- refer to [Catch2 CL ](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md) for more CL options


## How to Run

TBA
