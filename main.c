#include "rope.h"
#include <stdlib.h>
#include <string.h>

int main(){
    char str[] = "this is a rope";
    long len = strlen(str) / 2;
    char *left = str;
    char *right = str+len;
    str[len-1] = '\0';
    mem_for_special mem;
    init_mem_f_s(&mem, 1);

    rope_node *rope1 = concat(make_leaf(left), make_leaf(right));
    print_rope(rope1);
    free_ropes(rope1, &mem);
    free(mem.arr);

}
