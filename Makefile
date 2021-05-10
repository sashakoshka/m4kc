CC=gcc
CFLAGS=-Os -Wall
CFLAGS_COMPACT=-g0 -s -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables
LIBS=-L/usr/local/lib -lSDL2 -lm
PREFIX=/usr

.PHONY: clean install install-large copy-files run uninstall

m4kc:
	$(CC) main.c -o m4kc $(CFLAGS) $(CFLAGS_COMPACT) $(LIBS)
	strip m4kc -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag
	gzexe m4kc
	ls -l m4kc

m4kc-large:
	$(CC) main.c -o m4kc-large $(CFLAGS) $(LIBS)

install: m4kc copy-files
	install -m 755 m4kc $(DESTDIR)$(PREFIX)/bin/m4kc

install-large: m4kc-large copy-files
	install -m 755 m4kc-large $(DESTDIR)$(PREFIX)/bin/m4kc

copy-files:
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/share/applications/
	install -d $(DESTDIR)$(PREFIX)/share/icons/
	install -m 644 m4kc.desktop $(DESTDIR)$(PREFIX)/share/applications/m4kc.desktop
	install -m 644 icon.png $(DESTDIR)$(PREFIX)/share/icons/m4kc.png

run: clean m4kc-large
	./m4kc-large
	make clean

clean:
	rm -f m4kc m4kc~ m4kc-large

uninstall:
	rm "$(DESTDIR)$(PREFIX)/bin/m4kc" "$(DESTDIR)$(PREFIX)/share/applications/m4kc.desktop" "$(DESTDIR)$(PREFIX)/share/icons/m4kc.png"
