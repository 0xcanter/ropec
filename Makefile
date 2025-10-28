rope: rope.c test.c
	@gcc rope.c test.c -O2 -o test
run:
	./test
valgrind:
	@valgrind  ./test
