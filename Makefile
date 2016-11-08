# Not a real Makefile, just a little helper to test quickly on Windows

ifeq ($(OS),Windows_NT)
    EXE := .exe
else
    EXE :=
endif

all: test
t: test
test:
	echo $(OS)
	./build/Debug/shmoptix$(EXE) matte.sl

