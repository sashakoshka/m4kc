#!/bin/sh

# m4kc build system
# please keep this posix compliant!

CC="clang"
OBJ_PATH="o"
OUT_PATH="bin"
SRC_PATH="src"

FLAGS_SMALL="-Os -g0 -fno-stack-protector -fno-unwind-tables \
-fno-asynchronous-unwind-tables -Dsmall"
FLAGS_WARN="-Wall -Wextra"
FLAGS_DEBUG="-g"
FLAGS_LIBS="-L/usr/local/lib -lSDL2 -lm"

# do specific things if we are on windows

if [ ! -z "$MSYSTEM" ]; then
  FLAGS_INCLUDE="-mwindows -Iwin\SDL2\include"
  FLAGS_LIBS="-mwindows -Lwin\SDL2\lib -lmingw32 -lSDL2main -lSDL2"
  CC="gcc"
  
  OBJ_PATH="win/o"
  OUT_PATH="win/bin"
fi

SMALL_PATH="$OUT_PATH/m4kc"
DEBUG_PATH="$OUT_PATH/m4kc-debug"

# build a single module from src

buildModule () {
  mkdir -p "$OBJ_PATH"
  mkdir -p "$OBJ_PATH/small"
  mkdir -p "$OBJ_PATH/debug"

  modIn="$SRC_PATH/$1.c"
  modHead="$SRC_PATH/$1.h"

  flags="-c $FLAGS_WARN $FLAGS_INCLUDE"
  if [ "$2" = "small" ]
  then flags="$flags $FLAGS_SMALL"
       modOut="$OBJ_PATH/small/$1.o"
  else flags="$flags $FLAGS_DEBUG"
       modOut="$OBJ_PATH/debug/$1.o"
  fi
  
  if [ ! -f "$modIn" ]; then
  echo "!!! module $1 does not exist, skipping" >&2; return
  fi
  
  if [ "$modOut" -nt "$modIn" ] && [ "$modOut" -nt "$modHead" ]; then
  echo "(i) skipping module $1, already built"; return
  fi
  
  echo "... building module $1: $1.c ---> $1.o"
  
  $CC "$modIn" -o "$modOut" $flags && echo ".// built module $1" \
  || echo "ERR could not build module $1" >&2
}

# build all modules in src, then link them together into final executable

buildAll () {
  mkdir -p "$OUT_PATH"
  
  echo "... building all modules"

  for module in $SRC_PATH/*.c; do
    buildModule $(basename "${module%.*}") "$1"
  done

  echo "... building entire executable"
  
  flags="$FLAGS_LIBS $FLAGS_WARN"
  if [ "$1" = "small" ]
  then flags="$flags $FLAGS_SMALL -s"
       allIn="$OBJ_PATH/small/*.o"
       allOut="$SMALL_PATH"
  else flags="$flags $FLAGS_DEBUG"
       allIn="$OBJ_PATH/debug/*.o"
       allOut="$DEBUG_PATH"
  fi

  if $CC $allIn -o "$allOut" $flags
  then echo ".// built entire executable"
  else echo "ERR could not build executable" >&2
       return
  fi

  if [ "$1" = "small" ]; then
    echo "... compressing executable"

  	if strip "$SMALL_PATH" -S --strip-unneeded \
  	     --remove-section=.note.gnu.gold-version --remove-section=.comment \
  	     --remove-section=.note --remove-section=.note.gnu.build-id \
  	     --remove-section=.note.ABI-tag & \
  	   gzexe "$SMALL_PATH"
  	then
  	  ls -l "$SMALL_PATH"
      echo ".// compressed executable"
    else
      echo "ERR could not compress executable" >&2
    fi
  fi
  
  # copy SDL2.dll on windows
  if [ ! -z "$MSYSTEM" ]; then
    cp "win/SDL2.dll" "$OUT_PATH/SDL2.dll"
    echo "(i) copied SDL2.dll"
  fi
}

# clean everything

clean () {
  rm -f $OBJ_PATH/debug/* $OBJ_PATH/small/* $OUT_PATH/* && echo "(i) cleaned"
}

# control script

case $1 in
  all)   buildAll $2    ;;
  small) buildAll small ;;
  "")    buildAll       ;;

  redo)
    clean
    buildAll $2
    ;;
  
  clean)
    clean
    ;;
    
  run)
    buildAll && "./$DEBUG_PATH"
    ;;
    
  install)
    clean
    buildAll small                                                      \
    && echo "... installing"                                            \
    && install -d /usr/games/                                           \
    && install -d /usr/share/applications/                              \
    && install -d /usr/share/icons/                                     \
    && install -m 644 m4kc.desktop /usr/share/applications/m4kc.desktop \
    && install -m 644 icons/icon.png /usr/share/icons/m4kc.png          \
    && install -m 755 "./$SMALL_PATH" /usr/games/m4kc                   \
    && echo ".// installed"
    ;;
    
  uninstall)
    rm "/usr/games/m4kc"
    rm "/usr/share/applications/m4kc.desktop"
    rm "/usr/share/icons/m4kc.png"
    ;;
    
  *) buildModule $1 $2 ;;
esac
