"""Builds the SD-card zip of the release: python3 tools/package.py [version]
Needs build/balatro.nro (tools/build.sh). Writes dist/balatro-switch2-<version>.zip and dist/SHA256SUMS.txt."""
import hashlib
import os
import sys
import zipfile

root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
version = sys.argv[1] if len(sys.argv) > 1 else '1.0.0'
nro = os.path.join(root, 'build', 'balatro.nro')
if not os.path.exists(nro):
    sys.exit('build/balatro.nro is missing: run tools/build.sh first')

readme = """Balatro for Switch 2
====================
1. Put the game files of your own copy of Balatro into the game folder next to this file.
   The easiest way is the extract_game.py script of the project (see INSTALL.md): it unpacks Balatro.exe from Steam.
2. Start Balatro from the homebrew menu.

A Joy-Con on a table works as a mouse, and the touch screen works. Click both sticks together to turn the FPS counter
on or off. If the game does not start, the reason is in log.txt in this folder.
"""
put = "Put the game files of your own copy of Balatro here, so that main.lua is directly inside this folder.\n"

os.makedirs(os.path.join(root, 'dist'), exist_ok=True)
out = os.path.join(root, 'dist', 'balatro-switch2-%s.zip' % version)
with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
    for name, data in (('switch/balatro.nro', open(nro, 'rb').read()),
                       ('switch/balatro/README.txt', readme.encode()),
                       ('switch/balatro/game/PUT_BALATRO_FILES_HERE.txt', put.encode()),
                       ('switch/balatro/extract_game.py', open(os.path.join(root, 'tools', 'extract_game.py'), 'rb').read())):
        i = zipfile.ZipInfo(name, (2026, 1, 1, 0, 0, 0))
        i.compress_type = zipfile.ZIP_DEFLATED
        i.external_attr = 0o644 << 16
        z.writestr(i, data)
sha = hashlib.sha256(open(out, 'rb').read()).hexdigest()
open(os.path.join(root, 'dist', 'SHA256SUMS.txt'), 'w').write('%s  %s\n' % (sha, os.path.basename(out)))
print(out)
print(sha)
