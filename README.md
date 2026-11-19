# Balatro for Switch 2

**Balatro for the Nintendo Switch 2, as native homebrew at 120 frames per second, with touch screen and Joy-Con mouse.**

Balatro is a LOVE game. This is [LOVE 11.5](https://love2d.org) built for the Switch 2: OpenGL ES 3.0 on the console's own graphics driver, OpenAL for sound, and LuaJIT, linked against the Switch 2 runtime of the [ounce-devkit](https://github.com/ouncebrew/ounce-devkit). It runs **your own copy** of the game. The source code is in this repository and the build is on the [Releases](../../releases) page.

## How to play
1. Download `balatro-switch2-1.0.0.zip` from the [Releases](../../releases) page and unzip it onto the root of the console's SD card.
2. Put the game files of **your own** Steam copy of Balatro into `switch/balatro/game/`. The included `extract_game.py` does it for you: `python3 extract_game.py "C:\path\to\Balatro.exe" game`.
3. Start **Balatro** from the homebrew menu.

Nothing else to install. Your game files are never modified. Saves and settings are kept in `switch/balatro/save/`.

Step-by-step guide, including where every file goes: [INSTALL.md](INSTALL.md).

## Highlights
- Native Switch 2 homebrew, not a Switch 1 build in a compatibility layer
- 1920x1080 in handheld and docked mode, presented at the display's highest refresh rate: 120 Hz in handheld mode
- **Touch screen:** tap, drag and play with your fingers
- **Joy-Con mouse:** slide a Joy-Con 2 over a table to move the cursor, with L or ZL, R or ZR to click. A cursor is drawn where the game wants it
- Nintendo button layout: A on the right, B at the bottom, X at the top, Y on the left
- An FPS counter you can turn on while playing: click both sticks together. The choice is remembered
- Your own, unmodified game files

## Controls
The game's own controller support is used, with the Nintendo button layout.

| Input | What it does |
|---|---|
| Touch screen | tap and drag, like a mouse |
| Joy-Con mouse, sliding | move the cursor |
| Joy-Con mouse, L / ZL or R / ZR | click |
| Both stick clicks together | FPS counter on or off |

While a Joy-Con is used as a mouse, its shoulder and trigger count only as clicks, so the game does not switch to controller prompts.

## Known issues
- Sound is played through OpenAL; if you hear nothing, check `switch/balatro/log.txt`.
- The game's network features (leaderboards and the like) are not available. Steam features do not exist in this build.
- Online purchases and cloud saves do not exist; saves are local files.
- Docked mode renders 1920x1080 and the system scales it to the TV. A native 4K picture is not supported yet.

## Notes
- You must supply your own, legally obtained copy of Balatro. No game files are included.
- Requires a Nintendo Switch 2 running kaiHEN, with the homebrew menu.
- Build instructions: [BUILDING.md](BUILDING.md).

## Credits and licences
- [LOVE](https://love2d.org) by the LOVE Development Team, zlib licence (`LICENSE.love`).
- [LuaJIT](https://luajit.org) by Mike Pall, MIT (`LICENSE.luajit`).
- [kaiHEN](https://github.com/ouncebrew/kaihen) and the [ounce-devkit](https://github.com/ouncebrew/ounce-devkit) by ouncebrew.
- SDL2, OpenAL Soft and the other libraries from devkitPro's Switch portlibs.

The Switch 2 support code and the build scripts are released under the MIT licence (`LICENSE`). Balatro is a trademark of its owners. This project is not affiliated with or endorsed by them or by LocalThunk.
