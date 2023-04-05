# The Crow Programming Language
[[Website]](https://crowlang.org) [[Official Discord]](https://discord.gg/nBjBRQYK52)

```
(println "The Crow Programming Language")
(do-times (func () (println "Crow!")) 5)
```

## What is Crow

Crow is a Lisp like language written entirely in ANSI 1989 C. It is very fast (at least for purely interpreted) and features many features of both Lisp and other languages. Crow is both functional and object oriented, although the object oriented part is extremely limited for now.

## How to install

At the moment, to install Crow you need to build it from source. Nightly builds _are_ coming... some day...

## How to build Crow

### Windows - Visual Studio

To build on windows, download `cmake` and `Visual Studio`. Open a visual studio command prompt and `cd` to where Crow's source is stored. Then type

```
mkdir build
cd build
cmake ..
msbuild crow.sln
```

This should produce a Crow executable in the Debug folder. The solution file can also be opened in Visual Studio and compiled that way.

### Windows - MSys2

Simply follow the instructions for Unix

### Unix - Linux, MacOS, WSL, etc...

Simply type `make` in the root of the Crow source tree. For cmake, follow the same instructions as windows, of course not using `msbuild` but `make` instead.

## Documentation

I am in the process of writing documentation, but it call all be found in the [doc](/doc) folder.
