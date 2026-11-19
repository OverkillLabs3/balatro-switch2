"""Copies the game files out of your own copy of Balatro.

Usage: python3 tools/extract_game.py <path to Balatro.exe> <destination folder>

The Steam version of Balatro is a LOVE game: Balatro.exe is the LOVE program with the game's files (a zip archive)
attached at the end. This script unpacks that archive into the folder the Switch 2 build reads,
which is switch/balatro/game on the SD card. Nothing in the original file is changed."""
import os
import sys
import zipfile

if len(sys.argv) != 3:
    sys.exit(__doc__)

exe, dest = sys.argv[1], sys.argv[2]
try:
    z = zipfile.ZipFile(exe)
except zipfile.BadZipFile:
    sys.exit('%s has no game archive attached: is it the Balatro.exe of the Steam version?' % exe)

names = z.namelist()
if 'main.lua' not in names or 'game.lua' not in names:
    sys.exit('the archive in %s does not look like Balatro (no main.lua and game.lua)' % exe)

total = 0
for info in z.infolist():
    target = os.path.normpath(os.path.join(dest, info.filename))
    if not target.startswith(os.path.normpath(dest)):
        sys.exit('refusing to write outside the destination: ' + info.filename)
    if info.is_dir():
        os.makedirs(target, exist_ok=True)
        continue
    os.makedirs(os.path.dirname(target), exist_ok=True)
    with open(target, 'wb') as f:
        f.write(z.read(info))
    total += 1
print('%d files written to %s' % (total, dest))
