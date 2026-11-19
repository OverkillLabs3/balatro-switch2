# Installing Balatro on the Switch 2

You need a Nintendo Switch 2 running kaiHEN, a PC, the console's SD card and your own copy of Balatro from Steam. It takes about 5 minutes.

## 1. Your game files

The Steam version of Balatro is a LOVE game. `Balatro.exe` in the game's Steam folder is the LOVE program with the game's files attached to it as a zip archive. This port runs the files from that archive.

You can find the folder in Steam with **Manage**, **Browse local files**.

## 2. Put the port on the SD card

1. Download `balatro-switch2-1.0.0.zip` from the Releases page.
2. Put the console's SD card in your PC.
3. Unzip the file onto the **root** of the SD card. If asked, allow it to merge with the existing `switch` folder.

## 3. Copy your game files

You need Python 3 for the simple way. On the SD card, run the included script (change the paths):

```
python3 switch/balatro/extract_game.py "C:\Program Files (x86)\Steam\steamapps\common\Balatro\Balatro.exe" switch/balatro/game
```

or do it by hand: copy `Balatro.exe`, rename the copy to `Balatro.zip` and unzip it into `switch/balatro/game/`.

Either way, `switch/balatro/game/main.lua` must exist. You should now have:

```
SD card
└── switch
    ├── balatro.nro
    └── balatro
        ├── game
        │   ├── main.lua
        │   ├── game.lua
        │   ├── engine
        │   ├── resources
        │   └── (the rest of the game)
        └── save              (created by the game)
```

## 4. Play

1. Put the SD card back in the console and start the homebrew menu the way you normally do with kaiHEN.
2. Start **Balatro**.

If the game does not start, the reason is in `switch/balatro/log.txt` on the SD card.

## Tips

- Launch the game from a fresh homebrew menu. If another homebrew was started before it without restarting the menu, the graphics may not start; start the menu again.
- Tap the screen, or put a Joy-Con on a table and slide it to use it as a mouse.
- Click both sticks together to show or hide the FPS counter.
