gcc src\data.c -mwindows -Iwin\SDL2\include -c -o win\o\data.o
gcc src\gameloop.c -mwindows -Iwin\SDL2\include -c -o win\o\gameloop.o
gcc src\gui.c -mwindows -Iwin\SDL2\include -c -o win\o\gui.o
gcc src\inputbuffer.c -mwindows -Iwin\SDL2\include -c -o win\o\inputbuffer.o
gcc src\main.c -mwindows -Iwin\SDL2\include -c -o win\o\main.o
gcc src\menus.c -mwindows -Iwin\SDL2\include -c -o win\o\menus.o
gcc src\minecraftfont.c -mwindows -Iwin\SDL2\include -c -o win\o\minecraftfont.o
gcc src\player.c -mwindows -Iwin\SDL2\include -c -o win\o\player.o
gcc src\terrain.c -mwindows -Iwin\SDL2\include -c -o win\o\terrain.o
gcc src\textures.c -mwindows -Iwin\SDL2\include -c -o win\o\textures.o
gcc src\utility.c -mwindows -Iwin\SDL2\include -c -o win\o\utility.o

gcc win\o\* -mwindows -Lwin\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o win\bin\m4kc.exe
