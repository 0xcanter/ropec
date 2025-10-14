rope: rope.c main.c
	@gcc rope.c main.c -o test
run:
	./main
valgrind:
	@valgrind  ./main
