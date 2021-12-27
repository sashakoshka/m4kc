#!/bin/sh

# m4kc build system
# please keep this posix compliant!

CC="clang"
OBJ_PATH="o"
OUT_PATH="bin"
SRC_PATH="src"
SMALL_PATH="$OUT_PATH/m4kc"
DEBUG_PATH="$OUT_PATH/m4kc-debug"

FLAGS_SMALL="-Os -g0 -fno-stack-protector -fno-unwind-tables \
-fno-asynchronous-unwind-tables"
FLAGS_DEBUG="-g"
FLAGS_LIBS="-L/usr/local/lib -lSDL2 -lm"

# build a single module from src

buildModule () {
  mkdir -p "$OBJ_PATH"

  modOut="$OBJ_PATH/$1.o"
  modIn="$SRC_PATH/$1.c"
  modHead="$SRC_PATH/$1.h"
  
  if [ ! -f "$modIn" ]; then
  echo "!!! module $1 does not exist, skipping" >&2; return
  fi
  
  if [ "$modOut" -nt "$modIn" ] && [ "$modOut" -nt "$modHead" ]; then
  echo "(i) skipping module $1, already built"; return
  fi
  
  echo "... building module $1: $1.c ---> $1.o"

  flags="-c"
  if [ "$2" = "small" ]
  then flags="$flags $FLAGS_SMALL"
  else flags="$flags $FLAGS_DEBUG"
  fi
  
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
  
  flags="$FLAGS_LIBS"
  if [ "$1" = "small" ]
  then flags="$flags $FLAGS_SMALL -s"
       allOut="$SMALL_PATH"
  else flags="$flags $FLAGS_DEBUG"
       allOut="$DEBUG_PATH"
  fi

  if $CC $OBJ_PATH/*.o -o "$allOut" $flags
  then echo ".// built entire executable"
  else echo "ERR could not build executable" >&2
       return
  fi

  if [ "$1" = "small" ]; then
  	strip "$SMALL_PATH" -S --strip-unneeded \
  	      --remove-section=.note.gnu.gold-version --remove-section=.comment \
  	      --remove-section=.note --remove-section=.note.gnu.build-id \
  	      --remove-section=.note.ABI-tag
  	gzexe "$SMALL_PATH"
  	ls -l "$SMALL_PATH"
  fi
}

# clean everything

clean () {
  rm -f $OBJ_PATH/* $OUT_PATH/* && echo "(i) cleaned"
}

# control script

case $1 in
  all)   buildAll $2       ;;
  small) buildAll small    ;;
  "")    buildAll          ;;
  
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
