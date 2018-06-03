# chimera

[![Build Status](https://travis-ci.org/grandquista/chimera.svg?branch=master)](https://travis-ci.org/grandquista/chimera)

A forkable alternative Python3 interpreter.

## documentation

### compatibility

This fork tracks the latest cpython alpha spec for language features.  The stdlib can be cpython or pypy.

The threading model is different.  Garbage collection is a separate set of threads.  Also the method lookup model follows simpler rules.

### building/installing

The supported build process is cmake, ninja, and clang with support for c++17.  Default settings and target names are used.

### working on core

- [1 TOC](process/1_TOC.md) list of process documents

## forks

This is a list of forks that target compatibility with particular release versions of cpython.
