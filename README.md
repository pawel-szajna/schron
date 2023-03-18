# Schron

**Schron** is a yet-undetermined-genre (a mix of adventure, dungeon
crawling, horror, and who knows what more) game. Irregardless of the
genre, the player is put inside a shelter and his sanity is quickly
decaying.

## Prerequisites

* lua 5.3 (as the only dependency which caused problems when bundled)
* C++23 compiler (tested with g++ 13, Apple Clang 14, MSVC 19)
* CMake

### Parallel execution requirements

Some parts of the code take advantage of the parallel STL algorithms.
This can be disabled using the CMake `-DDISABLE_PARALLELISM=YES` option.
When enabled, the following additional requirements are introduced:

* Boost (header-only version is enough, as only `boost::iterator` is
  used), on all platforms
* TBB, UNIX platforms
* oneDPL, macOS

## Compilation

The compilation should be pretty straightforward. First of all (the
hardest part), lua should be somehow obtained. This might include your
package manager on Unix-like systems (apt, zypper, dnf, homebrew) or
something like vcpkg on Windows.

Most of the dependencies are compiled along the project (as this
helped resolve some conflicts with SDL versioning). To get those
dependencies, use git submodules:

```console
$ git submodule update --init --recursive
```

Afterwards, the usual steps of:

```console
$ mkdir build
$ cd build
$ cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
$ ninja schron
```

should produce a working binary.

## Execution

Some default assets (one might call this a demo version) are
provided with the game. Execute the compiled binary from the main
repository directory, and it should work out of the box.

## Scripting

The game uses LUA language quite extensively for scripting purposes.
At the moment, the scripting API is quite unstable and thus not yet
really documented.
