rope: rope.c
	@gcc rope.c -o rope
run:
	./rope
valgrind:
	@valgrind  ./rope
