# Schron

**Schron** is a yet-undetermined-genre (a mix of adventure, dungeon
crawling, horror, and who knows what more) game. Irregardless of the
genre, the player is put inside a shelter and his sanity is quickly
decaying.

## Prerequisites

* lua 5.3 (as the only one which caused problems when bundled)
* C++23 compiler (tested with `g++` 13)
* CMake

## Compilation

The compilation should be pretty straightforward. First of all (the
hardest part), lua should be somehow obtained. This might include your
package manager on Unix-like systems (apt, zypper, dnf, homebrew) or
some Windows magic on Windows.

Due to how different OSes install packages, you *might* have to update
the `CMakeLists.txt` (the main one, in the root directory) to help the
build system locate lua.

Most of the dependencies are compiled along the project (as this
helped resolve some conflicts with SDL versioning). To get those
dependencies, use git submodules:

```console
$ git submodule init
$ git submodule update
```

Afterwards, the usual steps of:

```console
$ mkdir build
$ cd build
$ cmake .. -GNinja
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
