# HoverBlast-2DShooter

2D Shooting Game in C Raylib

<!--toc:start-->
- [HoverBlast-2DShooter](#hoverblast-2dshooter)
  - [For the Web](#for-the-web)
    - [Requirements](#requirements)
    - [Build](#build)
    - [Run](#run)
<!--toc:end-->

## For the Web

### Requirements

- [emscripten](https://emscripten.org/docs/getting_started/downloads.html)
- [CMake](https://cmake.org/download/)

### Build

```sh
mkdir build && cd build
emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release
emmake make
```

### Run

```sh
emrun hover-blast
```

then open [https://localhost:6931] on your browser.
