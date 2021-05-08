all:
	gcc main.c -g0 -Wall -L/usr/local/lib -lSDL2 -lm -o m4kc -Os -s -fno-stack-protector -fno-unwind-tables -fno-asynchronous-unwind-tables
	strip m4kc -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag
	
	gzexe m4kc
	ls -la m4kc

all-large:
	gcc main.c -g0 -Wall -L/usr/local/lib -lSDL2 -lm -o m4kc

install:
	make all
	@echo "\n\nPlease enter your password to install the program:";
	sudo cp m4kc /bin/m4kc
	sudo cp m4kc.desktop /usr/share/applications/m4kc.desktop
	sudo cp icon.png /usr/share/icons/m4kc.png
	@echo "\n\nMinecraft 4k has been installed."
	@echo "You can type the command 'm4kc' to run it."

install-large:
	make all-large
	@echo  "\n\nPlease enter your password to install the program:";
	sudo cp m4kc /bin/m4kc
	@echo "\n\nMinecraft 4k has been installed (without size optimizations)."
	@echo "You can type the command 'm4kc' to run it."

run:
	make all-large
	./m4kc
