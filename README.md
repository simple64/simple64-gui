# mupen64plus-gui

mupen64plus-gui is written in Qt5. It supports everything you'd expect from a mupen64plus frontend (plugin selection, configuration editing, save state management, screenshots, pausing, etc..)

You can try this out by downloading [**m64p**](https://m64p.github.io/). **m64p** is mupen64plus + GLideN64 + this GUI

## Building (tested on Linux and MinGW)

On Ubuntu you'll want to install ```qt5-default```
```
git clone https://github.com/m64p/mupen64plus-gui.git
mkdir build
cd build
qmake ../mupen64plus-gui.pro
make -j4
```
then you can run ```./mupen64plus-gui```

## Some techincal details:

It requires Qt 5.4.

It is high-performance. The emulator runs in it's own thread. The game is displayed inside a QOpenGLWindow (native OpenGL Window, no extra rendering done by Qt).

## TODO

Controller configuration (auto-configuration already works).

Cheats.
