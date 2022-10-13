# chimera

[![ClangTidy](https://github.com/asakatida/chimera/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/clang-tidy.yml) [![CMake](https://github.com/asakatida/chimera/actions/workflows/cmake.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/cmake.yml) [![CodeCov](https://github.com/asakatida/chimera/actions/workflows/codecov.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/codecov.yml) [![CodeQL](https://github.com/asakatida/chimera/actions/workflows/codeql.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/codeql.yml) [![Docker](https://github.com/asakatida/chimera/actions/workflows/docker.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/docker.yml) [![Python](https://github.com/asakatida/chimera/actions/workflows/python.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/python.yml) [![TruffleHog](https://github.com/asakatida/chimera/actions/workflows/trufflehog.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/trufflehog.yml) [![YamlLint](https://github.com/asakatida/chimera/actions/workflows/yamllint.yml/badge.svg)](https://github.com/asakatida/chimera/actions/workflows/yamllint.yml)

A forkable alternative Python3 interpreter.

## documentation

### compatibility

This fork tracks the latest cpython alpha spec for language features.  The stdlib can be cpython or pypy.

The threading model is different.  Garbage collection is a separate set of threads.  Also the method lookup model follows simpler rules.

### building/installing

The supported build process is cmake, ninja, and clang with support for c++20.  Default settings and target names are used.

### working on core

- [1 TOC](process/1_TOC.md) list of process documents

## forks

This is a list of forks that target compatibility with particular release versions of cpython.
