#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#define CHUNK_SIZE 5
typedef struct rope_node{
    int weight;
    char *str;
    struct rope_node *left;
    struct rope_node *right;
}rope_node;

rope_node *make_leaf(char *str){
    rope_node *leaf = malloc(sizeof(rope_node));
    leaf->str = malloc(strlen(str)+1);
    strcpy(leaf->str,str);
    leaf->weight = strlen(str);
    leaf->left = leaf->right = NULL;
    return leaf;
}
int main (){
    char *str = "hello\0";
    rope_node *node = make_leaf(str);
    printf("%s world\n",node->str);
    free(node->str);free(node);
    return 0;
}
