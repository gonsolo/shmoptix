all: test

t: test
test: bla.ll
	cat $<
	
bla.ll: bla.cc
	clang -S -emit-llvm $<

.PHONY: c clean e edit

c: clean
clean:
	rm -f bla.ll
e: edit
edit:
	vi bla.cc bla.ll

