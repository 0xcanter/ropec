rope: rope.c test.c
	@gcc -W -g rope.c test.c -O2 -o test
run:
	./test
valgrind:
	@valgrind  ./test
test: rope.c test.c
	@gcc -W -g rope.c test.c -O2 -o test -fsanitize=address
