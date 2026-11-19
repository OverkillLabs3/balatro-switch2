#!/bin/bash
# Builds LOVE 11.5 for the Nintendo Switch 2 (SDL2, OpenGL ES 3.0, OpenAL, LuaJIT without the JIT) and checks the result.
# Needs devkitPro with the Switch 2 devkit and the portlibs listed in BUILDING.md.
set -e
R="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export DEVKITPRO="${DEVKITPRO:-/opt/devkitpro}"
export PATH="$DEVKITPRO/devkitA64/bin:$DEVKITPRO/tools/bin:$PATH"
unset CFLAGS CXXFLAGS CPPFLAGS LDFLAGS LIBS ARCH CC CXX LD AR
JOBS="$(nproc 2>/dev/null || echo 4)"

L="$R/third_party/love-11.5/src"
J="$R/third_party/LuaJIT/src"
O="$R/build/obj"
P="$DEVKITPRO/portlibs/switch"
P2="$DEVKITPRO/portlibs/switch2"
OUN="$DEVKITPRO/libounce"
mkdir -p "$O"

# 1. LuaJIT without the JIT: the console gives no executable memory to a homebrew program
if [ ! -f "$J/libluajit.a" ]; then
    TF="-mcpu=cortex-a78c+crypto -mtp=soft -fPIE -O2 -D__SWITCH__ -D__SWITCH2__ -isystem $OUN/include -fno-stack-protector"
    make -C "$J" libluajit.a CROSS=aarch64-none-elf- HOST_CC=gcc TARGET_SYS=Other TARGET_FLAGS="$TF" BUILDMODE=static \
        XCFLAGS="-DLUAJIT_DISABLE_JIT -DLUAJIT_SECURITY_PRNG=0 -DLUAJIT_USE_SYSMALLOC -DLUAJIT_NO_UNWIND"
fi

# 2. LOVE
COMMON="-mcpu=cortex-a78c+crypto -mtp=soft -fPIE -O2 -ffunction-sections -fdata-sections -D__SWITCH__ -D__SWITCH2__
 -DLOVE_LINUX=1 -DLOVE_GRAPHICS_USE_OPENGLES -D_GNU_SOURCE -w
 -isystem $OUN/include -I$L -I$L/modules -I$L/libraries -I$J
 -isystem $P/include/SDL2 -isystem $P/include/freetype2 -isystem $P/include/AL -isystem $P/include -isystem $P2/include"
export L O COMMON
export CXX_FLAGS="$COMMON -std=gnu++14 -fexceptions -frtti"

list_sources() {
    cd "$L"
    {
        echo love.cpp
        find common -name '*.cpp'
        for m in love audio data event filesystem font graphics image joystick keyboard math mouse sound system thread timer touch window physics video; do
            find modules/$m -maxdepth 1 -name '*.cpp'
        done
        find modules/audio/openal modules/audio/null modules/event/sdl modules/filesystem/physfs modules/font/freetype \
             modules/graphics/opengl modules/image/magpie modules/joystick/sdl modules/keyboard/sdl modules/mouse/sdl \
             modules/sound/lullaby modules/system/sdl modules/thread/sdl modules/touch/sdl modules/window/sdl \
             modules/physics/box2d modules/video/theora -name '*.cpp'
        find libraries/Wuff libraries/lodepng libraries/lz4 libraries/noise1234 libraries/utf8 libraries/xxHash \
             libraries/ddsparse libraries/stb libraries/Box2D libraries/glad libraries/glslang \( -name '*.c' -o -name '*.cpp' \)
        ls libraries/lua53/*.c
    } | sort -u | grep -v -E 'OSDependent/Windows|/android|/osx|macosx|/ios/|ddsparse/ddsview|/test|main\.cpp'
}

compile_one() {
    f="$1"
    o="$O/$(echo "$f" | tr '/' '_').o"
    [ -f "$o" ] && [ "$o" -nt "$L/$f" ] && return 0
    case "$f" in
        *.c)  aarch64-none-elf-gcc $COMMON -c "$L/$f" -o "$o" ;;
        *)    aarch64-none-elf-g++ $CXX_FLAGS -c "$L/$f" -o "$o" ;;
    esac || { echo "failed: $f"; return 1; }
}
export -f compile_one

list_sources > "$R/build/sources.txt"
echo "compiling $(wc -l < "$R/build/sources.txt") files"
xargs -P "$JOBS" -I{} bash -c 'compile_one {}' < "$R/build/sources.txt"

# the Switch 2 support code
aarch64-none-elf-gcc $COMMON -c "$R/src/switch.c" -o "$O/switch.o"

# 3. link, with a map file to see which libraries went in
cd "$R/build"
ls obj/*.o > objs.txt
aarch64-none-elf-g++ -mcpu=cortex-a78c+crypto -mtp=soft -fPIE -specs="$OUN/switch.specs" -Wl,--gc-sections -Wl,-Map,love.map \
    -L"$OUN/lib" -L"$P2/lib" -L"$P/lib" @objs.txt \
    -Wl,--start-group "$J/libluajit.a" -lSDL2 -lopenal -lvorbisfile -lvorbis -logg -lmodplug -lmpg123 -lfreetype -lharfbuzz \
    -lbz2 -lpng -lz -lphysfs -ltheoradec -lEGL -lglapi -ldrm_nouveau -lnx -lm -Wl,--end-group -o love.elf

# 4. NRO
aarch64-none-elf-strip -o love_stripped.elf love.elf
nacptool --create "Balatro" "OverkillLabs3" "1.0.0" balatro.nacp
elf2nro love_stripped.elf balatro.nro --icon="$R/src/icon.jpg" --nacp=balatro.nacp > /dev/null

# 5. checks
bash "$R/tools/verify_elf.sh" love.elf
python3 "$R/tools/verify_nro.py" balatro.nro love.map
echo "built $R/build/balatro.nro"
