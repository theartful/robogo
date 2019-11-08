# RoboGo

[![Build Status](https://travis-ci.com/Kareem-Emad/RoboGo.svg?token=ensmVBcwHV9ec6ob8Yqq&branch=master)](https://travis-ci.com/Kareem-Emad/RoboGo)

## Formatting

- Before submitting pull requests, make sure that you run clang-format so that
your code follows our standards, otherwise, travis will complain.

- If you already made your commit, you can run `git clang-format HEAD~1` to
format it, or you can use `clang-format <path-to-file> -i` from the beginning.

## Building

To build the project, simply create a build directory, go inside it, run cmake,
then start the building process by running make:

```shell
mkdir build
cd build
cmake ..
make
```

## Testing

To build unit tests, pass `-DENABLE_TESTING=ON` flag to cmake:

```shell
cmake -DENABLE_TESTING=ON ..
```

And after building the project, execute the following command to start running the tests:

```shell
make test
```

You can also run the resulting binary, which might give more flexibility.
Refer to
[Catch2 CL ](https://github.com/catchorg/Catch2/blob/master/docs/command-line.md)
for the CL options of the Catch2 test binary.

## How to Run

Make sure you are in build folder, and you built the project with cmake, then run the following command:

```shell
./bin/goslayer-executable
```

### CLI Supported Commands

- Make a move in position xy

```shell
mv xy
```

For example

```shell
mv F12
```

- Show board

```shell
board
```

- Show liberties map of cluster

```shell
lib xy
```

- Show cluster information

```shell
cluster xy
```

- Show game state (board, time, score ,, etc)

```shell
state
```