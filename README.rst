Schron
======

**Schron** is a yet-undetermined-genre (a mix of adventure, dungeon
crawling, horror, and who knows what more) game. Irregardless of the
genre, the player is put inside a shelter and his sanity is quickly
decaying.

Prerequisites
-------------

* lua 5.3+ (as the only dependency which caused problems when bundled)
* freetype (on Unix platforms; even though ``SDL_ttf`` includes this as
  a submodule, its CMakeLists expect it to be provided by system)
* C++23 compiler (tested with g++ 12.2, g++13, Apple Clang 14, MSVC 19)
* CMake

Debian prerequisites: ``apt install liblua54-dev libfreetype-dev``

openSUSE prerequisites: ``zypper in lua54-devel freetype-devel``

macOS prerequisites: ``brew install lua freetype``

Windows prerequisites: ``vcpkg install lua:x64-windows``

Parallel execution requirements
```````````````````````````````

Some parts of the code take advantage of the parallel STL algorithms.
This can be disabled using the CMake ``-DDISABLE_PARALLELISM=YES`` option.
When enabled, the following additional requirements are introduced:

* Boost (header-only version is enough, as only ``boost::iterator`` is
  used), on all platforms
* TBB, UNIX platforms
* oneDPL, macOS

Debian prerequisites: ``apt install libboost-dev libtbb-dev``

openSUSE prerequisites: ``zypper in boost-devel tbb-devel``

macOS prerequisites: ``brew install boost onedpl``

Windows prerequisites: ``vcpkg install boost:x64-windows``

Compilation
-----------

Most of the dependencies are compiled along the project (as this
helped resolve some conflicts with SDL versioning). To get those
dependencies, use git submodules::

    $ git submodule update --init --recursive

Afterwards, the usual steps, like::

    $ mkdir build
    $ cd build
    $ cmake .. -GNinja -DCMAKE_BUILD_TYPE=Release
    $ ninja schron

should produce a working binary.

Execution
---------

Some default assets (one might call this a demo version) are
provided with the game. Execute the compiled binary from the main
repository directory, and it should work out of the box.

Scripting
---------

The game uses LUA language quite extensively for scripting purposes.
At the moment, the scripting API is quite unstable, but some (more or
less accurate) description is provided in the docs.

