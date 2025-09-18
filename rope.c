#include "stdio.h"
#include <bits/time.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define CHUNK_SIZE 16
typedef struct rope_node{
    long weight;
    char *str;
    struct rope_node *left;
    struct rope_node *right;
}rope_node;

rope_node *make_leaf(char *str){
    size_t len = strlen(str);
    rope_node *leaf = malloc(sizeof(rope_node));
    leaf->weight
        = len;
    leaf->str = malloc(len+1);
    memcpy(leaf->str, str, len);
    leaf->str[leaf->weight] = '\0';
    leaf->left = leaf->right = NULL;
    return leaf;
}

rope_node *make_leaf_owned(char *str,long len){
    rope_node *leaf = malloc(sizeof(rope_node));
    leaf->weight = len;
    leaf->str = str;
    leaf->str[leaf->weight] = '\0';
    leaf->left = leaf->right = NULL;
    return leaf;
}

long length(rope_node *node){
    if (node == NULL)return 0;
    if (node->str != NULL ){
        return node->weight ;
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

rope_node *substr(rope_node *node,long start,long len){
    if(node == NULL || len <= 0 )return NULL;
    if(node->left == NULL && node->right == NULL && node->str != NULL){
        long leaf_len = node->weight;
        if(start >= leaf_len || start + len <= 0)return NULL;

        long copy_start = start < 0 ? 0:start;
        long copy_end = (start + len > leaf_len) ? leaf_len : start + len;
        long copy_len =  copy_end - copy_start;

        char *buf = malloc(copy_len + 1);
        memcpy(buf,node->str + copy_start,copy_len);
        buf[copy_len] = '\0';
        return make_leaf_owned(buf,copy_len);
    }

    long lef_len = (node->left) ? node->weight : 0;

    if(start + len <= lef_len){
        return substr(node->left, start, len);
    }else if(start >= lef_len){
        return substr(node->right, start - lef_len, len);
    }else{
        long lef_part_len = lef_len - start;
        rope_node *left = substr(node->left, start, lef_part_len);
        rope_node *right =  substr(node->right, 0, len - lef_part_len);
        return concat(left,right);
    }
}

void print_rope(rope_node *node) {
    if (node == NULL)return;
    if (node->str != NULL && node->left == NULL && node->right == NULL){
        printf("%s",node->str);
    }
    print_rope(node->left);
    print_rope(node->right);
}

void split_rope(rope_node *node,long pos,rope_node **left,rope_node **right){
    if(node == NULL){
        *left = NULL;
        *right = NULL;
        return;
    };
    if(node->left == NULL && node->right == NULL){
        if (pos <= 0){
            *left = make_leaf("");
            *right = node;
            return;
        }else if(pos >= node->weight){
            *left = node;
            *right = make_leaf("");
            return;
        }else if (pos <= node->weight){
            printf("haha");
            char *left_str = malloc(pos + 1);
            char *right_str = malloc((node->weight - pos) + 1);
            memcpy(left_str, node->str, pos);left_str[pos] = '\0';
            memcpy(right_str, node->str + pos, node->weight - pos);right_str[node->weight - pos] = '\0';
            *left = make_leaf_owned(left_str,pos);
            *right = make_leaf_owned(right_str,node->weight - pos);
            free(node->str);
            node->str = NULL;
            return;
        }
    }

    if (pos < node->weight){
        rope_node *L,*R;
        split_rope(node->left,pos, &L,&R);
        *left = L;
        *right = concat(R, node->right);
    }else if(pos == node->weight){
        *left = node->left;
        *right = node->right;
    }else{
        rope_node *L,*R;
        split_rope(node->right, pos - node->weight,&L,&R);
        *left = concat(node->left, L);
        *right = R;
    }

}

void insert(rope_node *node,long pos,char *text,rope_node **root){
    rope_node *left,*right;
    split_rope(node,pos,&left, &right);
    rope_node *insert_leaf = make_leaf(text);
    *root = concat(left, concat(insert_leaf, right));
}

void leaves_count(rope_node *node,long *i){
    if (node == NULL)return;
    if (node->str != NULL && node->left == NULL && node->right == NULL){
        *i += 1;
    }
    leaves_count(node->left,i);
    leaves_count(node->right,i);
}



int collect_leaves(rope_node *node,rope_node **leaves,int index){

    if (node == NULL){
        return index;
    }
    if (node->left == NULL && node->right == NULL){
        leaves[index++] = node;
        return index;
    }
   index = collect_leaves(node->left, leaves, index);

   index =  collect_leaves(node->right, leaves, index);
    return index;
}

rope_node *build_balanced_rope(rope_node **leaves,long n){
    if(n <= 0) return NULL;
    if(n == 1) return leaves[0];
    long leftn = n/2;
    long rightn = n - leftn;
    rope_node *left = build_balanced_rope(leaves, leftn);
    rope_node *right = build_balanced_rope(leaves + leftn, rightn);
    rope_node *parent = malloc(sizeof(rope_node));
    parent->str = NULL;
    parent->left = left;
    parent->right = right;
    parent->weight = left ? length(left):0;
    return parent;

}

void free_ropes(rope_node *root,int free_leaves){
    if (root == NULL){
        return;
    }
    free_ropes(root->left,free_leaves);
    free_ropes(root->right,free_leaves);
    if (root->str && free_leaves){
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

long fibonacci(long n){
    if(n == 0 )return 0;
    if(n == 1 )return 1;
    int a = 0,b = 1,c;
    for(int i = 2; i <= n; i++){
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

int is_balanced(rope_node *node){
    if (node == NULL) return 1;

    long rope_length = length(node);
    long depth_length = count_depth(node);
    long fib_depth = fibonacci(depth_length + 2);
    printf("\n%zu fib depth and rope %zu\n",fib_depth,rope_length);
    return rope_length >= fib_depth;
}

char find_char_rope(rope_node *node,long i){
    if(node == NULL)return '\0';
    if(i >= length(node)) return '\0';
    if(node->right == NULL && node->left == NULL){
        return node->str[i];
    }
    if(i < node->weight){
      return  find_char_rope(node->left, i );
    }else {

      return find_char_rope(node->right,i - node->weight);
    }
}


void copy_leaves(rope_node *n,char *buf,long *pos){
    if(!n)return;
    if(n->left == NULL && n->right == NULL && n->str != NULL){
        memcpy(buf + *pos, n->str,n->weight);
        *pos += n->weight;
        return;
    }
    copy_leaves(n->left,buf,pos);
    copy_leaves(n->right,buf,pos);
}

char *flatten_to_string(rope_node *node){
    if(node == NULL)return NULL;
    long total_len = length(node);
    char *buffer = malloc(total_len + 1);
    if(!buffer)return NULL;

    long offset = 0;

    copy_leaves(node,buffer,&offset);
    buffer[total_len] = '\0';
    return buffer;
}

struct timespec start,end;
int main (){
    FILE *f = fopen("test.txt", "r");
    if(!f){
        perror("failed to open file!");
        return 1;
    }
    char buffer[CHUNK_SIZE * 4 + 1];
    size_t bytes_read;

    rope_node *root = NULL;
    while ((bytes_read = fread(buffer, 1, CHUNK_SIZE * 4, f)) > 0) {
        buffer[bytes_read] = '\0';
        rope_node *leaf = make_leaf(buffer);
        if(root == NULL){
            root = leaf;
        }else {
            root = concat(root,leaf);
        }
    }
    fclose(f);

    long i = 0;
    leaves_count(root, &i);
    printf("%zu leaves",i);
    rope_node **leaves = malloc(sizeof(rope_node*) * i);

    int t =  collect_leaves(root, leaves, 0);

    rope_node *nd ;
    if(is_balanced(root) == 0){
        printf("\nrope is already balanced current depth is %zu\n",count_depth(root));
        nd = root;
    }else {
        printf("\nrope is not balanced\nbalancing now");
        nd = build_balanced_rope(leaves, i);
        printf("\ncurrent depth is %zu\n",count_depth(nd));
    }

    // char *flat_string = flatten_to_string(nd);
    // // printf("%s\n",flat_string);
    if (!nd){
        perror("failed to build a balanced rope");
        return 0;
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    rope_node *tem ;
    insert(nd,-2, " Chukwuka ",&tem);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1e9;
    print_rope(tem);
    printf("\nit took %.6f seconds\n", elapsed);
    printf("\n%zu characters in right and %zu characters in left \n",length(tem->right),tem->weight);
    printf("\n%zu total characters in the rope\n",length(tem->right) + tem->weight);
    char str = find_char_rope(tem, 383);
    printf("\n %c \n",str);
    char st = find_char_rope(tem, 185);
    printf("\n%c\n",st);
    free_internal(root);
    free(leaves);
    // free(flat_string);
    free_ropes(tem,1);
    return 0;
}
