rope: rope.c test.c
	@gcc rope.c test.c -o test
run:
	./test
valgrind:
	@valgrind  ./test
