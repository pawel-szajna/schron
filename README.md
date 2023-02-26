# Schron

**Schron** is a yet-undetermined-genre (a mix of adventure, dungeon
crawling, horror, and who knows what more) game. Irregardless of the
genre, the player is put inside a shelter and his sanity is quickly
decaying.

## Prerequisites

* `libsdl` (ancient version – 1.2 was used for development, might
  get replaced with a newer release one day)
* `libsprig`
* `spdlog`
* `sol2`
* C++23 compiler (tested with `g++` 13)
* CMake

## Compilation

Apart from getting the required packages installed (the hard step),
the compilation should be pretty straightforward. The usual steps
of:

```
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
