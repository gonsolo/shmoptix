# Not a real Makefile, just a little helper to test quickly on Windows

ifeq ($(OS),Windows_NT)
    EXE := .exe
    BUILD := RelWithDebInfo
    #BUILD := Debug
else
    EXE :=
endif

all: test
t: test
test:
	./build/$(BUILD)/shmoptix$(EXE) matte.sl

