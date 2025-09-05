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
    leaf->str = strdup(str);
    leaf->weight = strlen(str);
    leaf->left = leaf->right = NULL;
    return leaf;
}

int length(rope_node *node){
    if (node->str != NULL){
        return node->weight;
    }else{
        return node->weight + length(node->right);
    }
}

rope_node *concat(rope_node *left,rope_node *right){
    rope_node *node = malloc(sizeof(rope_node));
    node->left = left;
    node->right = right;
    node->str = NULL;
    node->weight = length(left);
    return node;
}

void print_rope(rope_node *node) {
    if (node == NULL)return;
    if (node->str != NULL){
        printf("%s",node->str);
    }
    print_rope(node->left);
    print_rope(node->right);
}

int main (){
    FILE *f = fopen("test2.txt", "r");
    if(!f){
        perror("failed to open file!");
        return 1;
    }
    char buffer[CHUNK_SIZE + 1];
    size_t bytes_read;

    rope_node *root = NULL;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE, f)) > 0) {
        buffer[bytes_read] = '\0';
        rope_node *leaf = make_leaf(buffer);

        if(root == NULL){
            root = leaf;
        }else {
            root = concat(root,leaf);
        }
    }

    print_rope(root);
    printf("%d",root->weight);
    return 0;
}
