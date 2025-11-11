#include "rope.h"
#include <stdio.h>
#include <string.h>
int main(){
    rope_node *rope;
    mem_for_special mem;
    init_mem_f_s(&mem, 1);

    const char *emojis1 = "😀😃😄😁😆😅😂🤣☺️😊";
    rope_append(&rope, emojis1);
    rope_node *d;
    delete_rope(rope,8 , &rope, 2, &mem, &d);
    unsigned char *flat = flatten_to_string(rope);
    printf("%s\n",strcmp((const char*)flat,"😀😃😄😁😆😅😂🤣😊") == 0 ? "PASS":"FAIL");
    printf("%zu \n",length(rope));
}
