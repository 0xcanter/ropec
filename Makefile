rope: rope.c
	@gcc -fsanitize=address -g -O1 -o rope rope.c;
run:
	./rope
valgrind:
	@valgrind  ./rope
