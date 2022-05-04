gcc src\data.c  -Iwin\SDL2\include -c -o win\o\data.o
gcc src\gameloop.c  -Iwin\SDL2\include -c -o win\o\gameloop.o
gcc src\gui.c  -Iwin\SDL2\include -c -o win\o\gui.o
gcc src\inputbuffer.c  -Iwin\SDL2\include -c -o win\o\inputbuffer.o
gcc src\main.c  -Iwin\SDL2\include -c -o win\o\main.o
gcc src\menus.c  -Iwin\SDL2\include -c -o win\o\menus.o
gcc src\minecraftfont.c  -Iwin\SDL2\include -c -o win\o\minecraftfont.o
gcc src\player.c  -Iwin\SDL2\include -c -o win\o\player.o
gcc src\terrain.c  -Iwin\SDL2\include -c -o win\o\terrain.o
gcc src\textures.c  -Iwin\SDL2\include -c -o win\o\textures.o
gcc src\utility.c  -Iwin\SDL2\include -c -o win\o\utility.o

gcc win\o\*  -Lwin\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o win\bin\m4kc.exe
