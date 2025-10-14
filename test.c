#include "rope.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

int main(){
    char str[] = "this is a rope";
    long len = strlen(str) / 2;
    char *left = str;
    char *right = str+len;
    rope_node *temp2 = make_leaf(right);
    str[len] = '\0';
    rope_node *temp = make_leaf(left);
    mem_for_special mem;
    init_mem_f_s(&mem, 1);

    rope_node *rope1 = concat(temp, temp2);
    if(rope1 == NULL) return 1;
    insert_rope(rope1, 5, "temple ", &rope1, &mem);
    print_rope(rope1);printf("\n");
    free_ropes(rope1, &mem);
    return 0;
}
