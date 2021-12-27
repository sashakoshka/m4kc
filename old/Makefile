CC=clang
WARN=-Wall -Wextra
COMPACT=-Os -g0 -s -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables
DEBUG=-g
LIBS=-L/usr/local/lib -lSDL2 -lm

.PHONY: warning all all-debug install run clean uninstall

warning:
	@echo "DO NOT USE THIS FILE!!!!!!"
	@echo "A NEW SCRIPT (build.sh) HAS BEEN MADE"

all:
	mkdir -p bin
	$(CC) src/*.c -o bin/m4kc $(WARN) $(LIBS) $(COMPACT)
	strip bin/m4kc -S --strip-unneeded --remove-section=.note.gnu.gold-version \
	      --remove-section=.comment --remove-section=.note                     \
	      --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag
	gzexe bin/m4kc
	ls -l bin/m4kc

all-debug:
	mkdir -p bin
	$(CC) src/*.c -o bin/m4kc-debug $(WARN) $(LIBS) $(debug)

install: all
	install -d /usr/games/
	install -d /usr/share/applications/
	install -d /usr/share/icons/
	install -m 644 m4kc.desktop /usr/share/applications/m4kc.desktop
	install -m 644 icons/icon.png /usr/share/icons/m4kc.png
	install -m 755 bin/m4kc /usr/games/m4kc

run: all-debug
	./bin/m4kc-debug

clean:
	rm -f bin/*

uninstall:
	rm "/usr/games/m4kc"
	rm "/usr/share/applications/m4kc.desktop"
	rm "/usr/share/icons/m4kc.png"
