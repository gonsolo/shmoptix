# Not a real Makefile, just a little helper to test quickly on Windows

all: test
t: test
test:
	./build/Debug/shmoptix.exe matte.sl

