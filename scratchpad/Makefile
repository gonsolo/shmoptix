all: test

LLC:=../../../src/llvm-3.9.0.src/build/Debug/bin/llc.exe

testMatte: testMatte.c matte.s
	gcc -o testMatte testMatte.c matte.s

matte.s: matte.ll
	$(LLC) matte.ll

#t: test
#test: bla.ll
#	cat $<
	
bla.ll: bla.cc
	clang -S -emit-llvm $<

.PHONY: c clean e edit

c: clean
clean:
	rm -f bla.ll testMatte.exe matte.s
e: edit
edit:
	vi bla.cc bla.ll
t: test
test: testMatte
	./testMatte

