# Building

LOVE is built with devkitPro and the Switch 2 devkit (libounce, OunceVK), on Windows (the devkitPro MSYS2 shell) or on Linux.

## Requirements

- [devkitPro](https://devkitpro.org/wiki/Getting_Started) with the `switch-dev` group
- the [ounce-devkit](https://github.com/ouncebrew/ounce-devkit), installed into the same devkitPro (run its `install.sh`)
- these portlibs, installed with `pacman -S`: `switch-freetype switch-harfbuzz switch-libvorbis switch-libogg switch-openal-soft switch-libmodplug switch-mpg123 switch-physfs switch-libtheora switch-libpng switch-bzip2 switch-zlib switch-sdl2`
- `git`, `curl`, `tar`, `python3`, `make` and a host `gcc` (LuaJIT builds a small tool for the machine it runs on)
- `DEVKITPRO` set (the default is `/opt/devkitpro`)

## Steps

```sh
bash tools/fetch_sources.sh     # LOVE 11.5 and LuaJIT, pinned, into third_party/
bash tools/apply_patches.sh     # the Switch 2 changes to LOVE
bash tools/build.sh             # builds, checks and writes build/balatro.nro
python3 tools/package.py        # the SD-card zip in dist/
```

`build/balatro.nro` is the program. `tools/build.sh` links with a map file and runs two checks on the result:

- `tools/verify_elf.sh` refuses a program that was not linked against the Switch 2 runtime or that carries the Switch 1 graphics libraries
- `tools/verify_nro.py` checks the NRO header and, with the map, that no Switch 1 library went into the link

A program built against the Switch 1 libnx can make the console warn that it may crash the system, so do not skip these checks.

## Layout

| Path | What |
|---|---|
| `patches/` | the changes to LOVE: OpenGL ES 3.0 only, the game folder on the SD card, the Joy-Con mouse and touch screen events, the Nintendo button layout, the 120 Hz request, the cursor and the FPS counter |
| `src/switch.c` | the Switch 2 code: start-up, 120 Hz request, Joy-Con mouse and touch screen |
| `src/icon.jpg` | the icon of the program |
| `tools/` | fetch, patch, build, check, package and game extraction scripts |

LuaJIT is built without its JIT compiler, because a homebrew program gets no executable memory from the console.
