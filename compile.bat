gcc -Wall -Werror -m64 -ffreestanding -DPRINT -c example.c -o example.o
gcc -Wall -Werror -m64 -ffreestanding -DPRINT -c JSON.c -o JSON.o
gcc -Wall -Werror -m64 -ffreestanding -DPRINT -c Common-Code-For-C-Projects\memory.c -o memory.o

gcc -Wall -Werror -m64 example.o JSON.o memory.o -o example.exe
.\example.exe .\test.json

del *.o

pause