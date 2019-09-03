@echo off

gcc -std=c11 -Wall -m64 -ggdb2 -O3 -c -municode -Llib -Iinclude src/winapi.c -o build/winapi.o 
windres -i src/podcaster.rc -o build/resource.o
gcc -Llib -Iinclude build/winapi.o build/resource.o -o podded.exe -luser32 -lgdi32 -lcomctl32 -lcurl -lxml2 -lsqlite3 -lole32