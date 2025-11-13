#include "rope.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
int main(){
    rope_node *rope;
    rope = NULL;
    mem_for_special mem;
    init_mem_f_s(&mem, 1);

    static unsigned char *emojis1 = (unsigned char*)"😀😃✅😄😁😆😅😂🤣☺️😊";
    rope_append(&rope, (const char *)emojis1);
    rope_node *d;
    clock_t start,end;
    double total;
    start = clock();
    const unsigned char *t = find_char_rope(rope, 11);
    printf("%s\n",t);
    end = clock();
    total = (double)(end - start)/CLOCKS_PER_SEC;
    printf("%.6f\n",total);
    delete_rope(rope,8 , &rope, 2, &mem, &d);
    
    unsigned char *flat = flatten_to_string(rope);
    print_rope(rope);
    printf("%s\n",strcmp((const char*)t,"😂") == 0 ? "PASS":"FAIL");
    printf("%zu \n",length(rope));
}
