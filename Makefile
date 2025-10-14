rope: rope.c main.c
	@gcc rope.c main.c -o test
run:
	./test
valgrind:
	@valgrind  ./test
