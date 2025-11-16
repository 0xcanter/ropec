#include "rope.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
int main(){
    rope_node *rope;
    rope = NULL;
    mem_for_special mem;
    init_mem_f_s(&mem, 1);

    static unsigned char *emojis1 = (unsigned char*)"hello my name is temple and\n"
    "my name is this temple that i dont think you understand\n"
    "thats all from me and i get it\n"
    "this is not my type of thing and i dont know\n"
    "this is my favorite thing to do\n";

    rope_append(&rope, (const char *)emojis1);
    rope_node *d;
    clock_t start,end;
    double total;
    start = clock();
    insert_rope(rope, 6, "✅", &rope, &mem);
    insert_rope(rope, 7, "✅", &rope, &mem);
    rebalance(rope, &rope, &mem);
    const unsigned char *t = find_char_rope(rope, 11);
    printf("%s\n",t);
    end = clock();
    total = (double)(end - start)/CLOCKS_PER_SEC;
    printf("%.6f\n",total);
    
    unsigned char *flat = flatten_to_string(rope);
    printf("flat: %s\n",flat);
    print_rope(rope->right);
    printf("%zu \n",rope->line_count);
    printf("%zu\n",find_start(rope, 2));
    return 0;
}
