# RoboGo

[![Build Status](https://travis-ci.com/Kareem-Emad/RoboGo.svg?token=ensmVBcwHV9ec6ob8Yqq&branch=master)](https://travis-ci.com/Kareem-Emad/RoboGo)

## Formatting

- Before submitting pull requests, make sure that you run clang-format so that
your code follows our standards, otherwise, travis will complain.

- If you already made your commit, you can run `git clang-format HEAD~1` to
format it, or you can use `clang-format <path-to-file> -i` from the beginning.

## Testing

- to run tests

```shell
g++ tests/*.cpp -lCppUTest
./a.out
rm a.out
```

- you need to have [cpputest](https://cpputest.github.io/manual.html) installed

## How to Run

TBA
