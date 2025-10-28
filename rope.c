#include "stdio.h"
// #include <bits/time.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include "rope.h"





void init_mem_f_s(mem_for_special *mem,size_t initial_cap){
    if(initial_cap <= 0)return;
    mem->arr = calloc(initial_cap,sizeof(rope_node*));
    mem->size = 0;
    mem->cap = initial_cap;
}


int exists_in_mem(rope_node *node,mem_for_special *mem) {
    if(node == NULL || mem->cap <= 0)return -1;
    for (size_t i = 0; i < mem->size; i++) {
        if (mem->arr[i] == node) {
            return 1;
        }
    }
    return 0;
}

void free_mem(mem_for_special *mem){
    if(mem->arr == NULL){
        perror("mem.arr is null");
        return;
    }
       for(int i = 0;i<mem->size;i++){
        rope_node *node = mem->arr[i];
        // if(!node)perror("hey thats\n");
        if(!node)continue;
         if (node->str){
            free(node->str);
            node->str = NULL;
        }
        free(node);
        // mem->arr[i] = NULL;
    }
    free(mem->arr);
    mem->arr = NULL;
    mem->cap = 0;
    mem->size = 0;
}

void add_to_mem(mem_for_special *mem,rope_node *node){
    if(node == NULL){
        perror("node is null");
        return;
    }
    if(mem == NULL){
        perror("mem is null");
        return;
    }

    if(mem->size >= mem->cap){
        size_t old_cap = mem->cap;
        mem->cap = mem->cap * 2;
        rope_node **tmp = realloc(mem->arr, mem->cap * sizeof(rope_node*));
        if(!tmp){
            perror("mem realloc failed!");
            exit(EXIT_FAILURE);
        }
        mem->arr = tmp;
        memset(mem->arr + old_cap, 0 , (mem->cap - old_cap) * sizeof(rope_node*));
    }
    if (!exists_in_mem(node,mem)) {
        mem->arr[mem->size++] = node;
    }
}

rope_node *make_leaf(const char *str){
    if(str == NULL){
        perror("error making leaf!: string is null");
        return NULL;
    }
    size_t len = strlen(str);
    // printf("\n%zu first leaf\n",len);
    rope_node *leaf = malloc(sizeof(rope_node));
    if(leaf == NULL){
        perror("failed to allocate memory for leaf!");
        return NULL;
    }

    size_t l_count = 0;
    for(int i = 0;i<len;i++){
        if(str[i] == '\n'){
            l_count++;
        }
    }

    leaf->line_count = l_count;
    if(!leaf)return NULL;
    leaf->weight
        = len;
    leaf->str = malloc(len+1);
    if (!leaf->str) {
        free(leaf);
        return NULL;
    }
    memcpy(leaf->str, str, len);
    leaf->str[leaf->weight] = '\0';
    leaf->left = leaf->right = NULL;
    return leaf;
}

rope_node *make_leaf_owned(char *str,long len){
    if(str == NULL){
        perror("failed to make leaf : str is null");
        return NULL;
    }
    rope_node *leaf = malloc(sizeof(rope_node));
    if(leaf == NULL){
        perror("failed to allocate memory for leaf");
        return NULL;
    }
    
    size_t l_count = 0;
    for(int i = 0;i<len;i++){
        if(str[i] == '\n'){
            l_count++;
        }
    }

    leaf->line_count = l_count;
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

size_t lines(rope_node *node){
    if (node == NULL)return 0;
    if (node->str != NULL){
        return node->line_count;
    }
    return lines(node->left) + lines(node->right);
}                        

rope_node *concat(rope_node *left,rope_node *right){
    if(left == NULL || right == NULL){
        perror("error concatenating node the left or right node is null");
        return NULL;
    }
    rope_node *node = malloc(sizeof(rope_node));
    if(node == NULL){
        perror("failed to allocate memory for node");
        return NULL;
    }
    node->left = left;
    node->right = right;
    node->str = NULL;
    node->line_count = left->line_count + right->line_count;
    node->weight = length(left);
    return node;
}

rope_node *substr(rope_node *node,long start,long len){
    if(node == NULL || len <= 0 ){
        perror("error in substring: node is null or len is <= 0");
        return NULL;
    }
    if(node->left == NULL && node->right == NULL && node->str != NULL){
        long leaf_len = node->weight;
        if(start >= leaf_len || start + len <= 0)return NULL;

        long copy_start = start < 0 ? 0:start;
        long copy_end = (start + len > leaf_len) ? leaf_len : start + len;
        long copy_len =  copy_end - copy_start;

        char *buf = malloc(copy_len + 1);
        if(buf == NULL){
            perror("failed to allocate memory for buffer");
            return NULL;
        }
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
    if (node == NULL){
        return;
    };
    if (node->str != NULL && node->left == NULL && node->right == NULL){
        printf("%s",node->str);
    }
    print_rope(node->left);
    print_rope(node->right);
}


void fast_substr(rope_node *node,long start,long end,rope_node **newsub,mem_for_special *mem){

    if(node == NULL){
        perror("error getting substring of the node: node is null");
        return;
    }
    
    long len = length(node);
    long new_end;
    
    if(start < 0 || start > len){
        perror("start cant be greater than length or less than 0");
        return;
    }
    
    rope_node *l,*r;
    split_rope(node,start ,&l, &r,mem);

    long remains = len - start;
    if(end > remains){
       new_end = remains;
    }else new_end = end;
    
    rope_node *L,*R;
    split_rope(r, new_end, &L,&R ,mem);
    *newsub = L;
    add_to_mem(mem, r);
    add_to_mem(mem, l);
    add_to_mem(mem, R);
    add_to_mem(mem, L);
    return;    
   
    
}

void split_rope(rope_node *node,long pos,rope_node **left,rope_node **right,mem_for_special *mem){
    if(node == NULL){
        *left = NULL;
        *right = NULL;
        return;
    };
    if(node->left == NULL && node->right == NULL){
        if (pos <= 0){
            *left = NULL;
            *right = node;
            return;
        }else if(pos >= node->weight){
            *left = node;
            *right = NULL;
            return;
        }else if (pos <= node->weight){
            char *left_str = malloc(pos + 1);
            char *right_str = malloc((node->weight - pos) + 1);
            memcpy(left_str, node->str, pos);left_str[pos] = '\0';
            memcpy(right_str, node->str + pos, node->weight - pos);right_str[node->weight - pos] = '\0';
            *left = make_leaf_owned(left_str,pos);
            *right = make_leaf_owned(right_str,node->weight - pos);
            add_to_mem(mem, *left);
            add_to_mem(mem, *right);
            add_to_mem(mem,node);
            return;
        }
    }

    if (pos < node->weight){
        rope_node *L,*R;
        split_rope(node->left,pos, &L,&R,mem);
        *left = L;
        *right = concat(R, node->right);
        add_to_mem(mem,node);
        add_to_mem(mem, *left);
        add_to_mem(mem, *right);
    }else if(pos == node->weight){
        *left = node->left;
        *right = node->right;
        add_to_mem(mem,node);
    }else{
        rope_node *L,*R;
        split_rope(node->right, pos - node->weight,&L,&R,mem);
        *left = concat(node->left, L);
        *right = R;
        add_to_mem(mem, *left);
        add_to_mem(mem, *right);
        add_to_mem(mem,node);
    }

}

void insert_rope(rope_node *node,long pos,char *text,rope_node **root,mem_for_special *mem){
    if(node == NULL){
        perror("failed to insert rope: rope is null");
        return;
    }
    rope_node *left,*right;
    split_rope(node,pos,&left, &right,mem);
    rope_node *insert_leaf = make_leaf(text);
    add_to_mem(mem, node);
    *root = concat(left, concat(insert_leaf, right));
}

void delete_rope(rope_node *node,long pos,rope_node **root,long len,mem_for_special *mem,rope_node **deleted){
    if (!node || len <= 0){
        *root = node;
        perror("error deleting node: node is null");
        return;
    };
    long total = length(node);
    if (pos < 0) pos = 0;
    if(pos >= total){
        *root = node;
        return;
    }
    long right_start = pos + len;
    if(right_start > total)len = total - pos;
    rope_node *left,*right,*temp,*ignore;
    split_rope(node, pos, &left, &temp,mem);
    split_rope(temp, len, &ignore, &right,mem);
    *deleted = ignore;
    add_to_mem(mem, temp);
    *root = concat(left, right);
}

void leaves_count(rope_node *node,int *i){
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
    if (node->left == NULL && node->right == NULL&& node->str != NULL){
        leaves[index++] = node;
        return index;
    }
   index = collect_leaves(node->left, leaves, index);

   index =  collect_leaves(node->right, leaves, index);
    return index;
}

void rebalance(rope_node *node,rope_node **root,mem_for_special *mem){
    int count = 0;
    printf("for node %lu\n",count_depth(node));

    printf("for right %lu\n",count_depth(node->right));

    printf("for left %lu\n", count_depth(node->left));
    
    leaves_count(node, &count);
    printf("%d\n",count);
    rope_node **leaves = malloc(sizeof(rope_node*) * count);
    count = 0;
    count = collect_leaves(node, leaves,0);
    printf("%d\n",count);
    if(leaves == NULL)return;
    
    *root = build_balanced_rope(leaves, count);
    // add_to_mem(mem,node);
    free_internal(node,mem);
    free(leaves);
}

rope_node *build_balanced_rope(rope_node **leaves,long n){
    if(n <= 0) return NULL;
    if(n == 1) return leaves[0];
    long leftn = n/2;
    long rightn = n - leftn;
    rope_node *left = build_balanced_rope(leaves, leftn);
    rope_node *right = build_balanced_rope(leaves + leftn, rightn);
    rope_node *parent = malloc(sizeof(rope_node));
    if(parent == NULL){
        perror("error building balanced rope: memory allocation for parent node failed");
        return NULL;
    }
    parent->str = NULL;
    parent->left = left;
    parent->right = right;
    parent->line_count = (left ? lines(left):0)+(right ? lines(right):0);
    parent->weight = left ? length(left):0;
    return parent;

}

void free_ropes(rope_node *root,mem_for_special *mem){
    if (root == NULL){
        return;
    }
    free_ropes(root->left,mem);
    root->left = NULL;
    free_ropes(root->right,mem);
    root->right = NULL;
    if (root->str){
        free(root->str);
        root->str = NULL;
    }
    // free(root);
    if(exists_in_mem(root,mem) == 0){
        free(root);
        root = NULL;
    }

}



void free_internal(rope_node *node,mem_for_special *mem){
    if (node == NULL){
        return;
    }
    free_internal(node->left,mem);
    free_internal(node->right,mem);

    if(node->str == NULL){
         if(exists_in_mem(node,mem) == 0){
            free(node);
            node = NULL;
        }
    };
    // free(node);
   

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

unsigned long long fibonacci(long n){
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
    if (node == NULL) return 2;
    
    printf("for node %lu\n",count_depth(node));

    printf("for right %lu\n",count_depth(node->right));

    printf("for left %lu\n", count_depth(node->left));
    long rope_length = length(node);
    long depth_length = count_depth(node);
    unsigned long long fib_depth = fibonacci(depth_length + 2);
    printf("\n%llu fib depth and rope %ld\n",fib_depth,rope_length);
    return rope_length >= fib_depth ? 1:0;
}

char find_char_rope(rope_node *node,long i){
    if(node == NULL) return '\0';
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
    if(n->left == NULL && n->right == NULL){
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
                
void rope_append(rope_node **root,const char *str){
    rope_node *leaf = make_leaf(str);
    if(*root == NULL){
        *root = leaf;
        return;
    }
    *root = concat(*root, leaf);
}
