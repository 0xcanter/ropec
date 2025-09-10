#include "stdio.h"
#include <bits/time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CHUNK_SIZE 3000
typedef struct rope_node{
    long weight;
    char *str;
    struct rope_node *left;
    struct rope_node *right;
}rope_node;

rope_node *make_leaf(char *str){
    rope_node *leaf = malloc(sizeof(rope_node));
    leaf->str = malloc(strlen(str)+1);
    strncpy(leaf->str, str, strlen(str)+1);
    leaf->weight = strlen(str);
    leaf->left = leaf->right = NULL;
    return leaf;
}

// rope_node *substr(rope_node *rope,int start,int len){
//     if()
// }

long length(rope_node *node){
    if (node == NULL)return 0;
    if (node->str != NULL ){
        return node->weight;
    }
    return node->weight + length(node->right);

}

rope_node *concat(rope_node *left,rope_node *right){
    rope_node *node = malloc(sizeof(rope_node));
    node->left = left;
    node->right = right;
    node->str = NULL;
    node->weight = length(left);
    return node;
}

    int i = 0;
void print_rope(rope_node *node) {
    if (node == NULL)return;
    if (node->str != NULL && node->left == NULL && node->right == NULL){
        // printf("%s",node->str);
        // printf("%zu\n",strlen(node->str));
        i++;
    }
    print_rope(node->left);
    print_rope(node->right);
}
int depth = 0;
int collect_leaves(rope_node *node,rope_node **leaves,int index){

    if (node == NULL){
        return index;
    }
    if (node->left == NULL && node->right == NULL){
        leaves[index++] = node;
        return index;
    }
   index = collect_leaves(node->left, leaves, index);
    depth++;
   index =  collect_leaves(node->right, leaves, index);
    return index;
}

rope_node *build_balanced_rope(rope_node **leaves,int n){
    if(n <= 0) return NULL;
    if(n == 1) return leaves[0];
    int leftn = n/2;
    int rightn = n - leftn;
    rope_node *left = build_balanced_rope(leaves, leftn);
    rope_node *right = build_balanced_rope(leaves + leftn, rightn);
    rope_node *parent = malloc(sizeof(rope_node));
    parent->str = NULL;
    parent->left = left;
    // parent->left->weight = length(left);
    parent->right = right;
    // parent->right->weight = length(right);
    parent->weight = left ? length(left):0;
    return parent;

}

void free_ropes(rope_node *root,int free_leaves){
    if (root == NULL){
        return;
    }
    free_ropes(root->left,free_leaves);
    free_ropes(root->right,free_leaves);
    if (root->left == NULL && root->right == NULL){
        if(free_leaves && root->str){
            free(root->str);
        }
    }
    free(root);
}

void free_internal(rope_node *node){
    if (node == NULL){
        return;
    }
    free_internal(node->left);
    free_internal(node->right);

    if(node->str == NULL)free(node);

}

long count_depth(rope_node *node){
    if (node == NULL){
        return 0;
    }
    if (node->left == NULL && node->right == NULL)return 1;
    int left_depth = count_depth(node->left);
    int right_depth = count_depth(node->right);

    return 1 + (left_depth > right_depth ? left_depth:right_depth);
}


struct timespec start,end;
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
    fclose(f);

    // print_rope(root->left);
    // print_rope(root->right);
    // printf("%zu for root node\n",root->weight);
    print_rope(root);
    rope_node **leaves = malloc(sizeof(rope_node*)*i);
    clock_gettime(CLOCK_MONOTONIC, &start);
    int t =  collect_leaves(root, leaves, 0);

    // for (int j = 0; j<i; j++) {
    //     rope_node *n = leaves[j];
    //     printf("\n%d\n",n->weight);
    // }

    printf("%di",i);
    rope_node *nd = build_balanced_rope(leaves, i);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Flatten and rebalancing took %.6f seconds\n", elapsed);
    if (!nd){
        perror("failed to build a balanced rope");
        return 0;
    }
    print_rope(nd);
    printf("\n%zu characters in right and %zu characters in left \n",length(nd->right),length(nd->left));
    printf("\n%zu total characters in the rope\n",nd->weight);
    // printf("\n%zu",nd->left->weight );
    long rdepth = count_depth(root);
    long ndepth = count_depth(nd);

    printf("\n%d total leaves\n",i);
    printf("%zu total depth before rebalancing and %zu after rebalancing ",rdepth,ndepth);
    free_internal(root);
    free(leaves);
    free_ropes(nd,1);
    return 0;
}
