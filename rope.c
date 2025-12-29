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
        // perror("mem.arr is null");
        return;
    }
       for(size_t i = 0;i<mem->size;i++){
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
        // perror("node is null");
        return;
    }
    if(mem == NULL){
        // perror("mem is null");
        return;
    }

    if(mem->size >= mem->cap){
        size_t old_cap = mem->cap;
        mem->cap = mem->cap * 2;
        rope_node **tmp = realloc(mem->arr, mem->cap * sizeof(rope_node*));
        if(!tmp){
            // perror("mem realloc failed!");
            exit(EXIT_FAILURE);
        }
        mem->arr = tmp;
        memset(mem->arr + old_cap, 0 , (mem->cap - old_cap) * sizeof(rope_node*));
    }
    if (!exists_in_mem(node,mem)) {
        mem->arr[mem->size++] = node;
    }
}


size_t utf8_char_len(const unsigned char *string){
    size_t count = 0;
    while(*string){
        unsigned char c = (unsigned char)*string;
        if((c & 0xC0) != 0x80) {
            count++;
        }
        string++;
    }
    return count;
}

size_t utf8_char_byte(const unsigned char *string,size_t char_index){
    size_t count = 0;
    size_t byte_index = 0;
    while(string[byte_index] != '\0'){
        if((string[byte_index] & 0xC0) != 0x80){
            if (count == char_index)return byte_index;
            count++;
        }
        byte_index++;
    }
    return byte_index;
}

rope_node *make_leaf(const char *str){
    if(str == NULL){
        // perror("error making leaf!: string is null");
        return NULL;
    }
    size_t len = utf8_char_len((const unsigned char *)str);
    size_t byte_count = strlen(str);
    rope_node *leaf = calloc(1,sizeof(rope_node));
    if(leaf == NULL){
        // perror("failed to allocate memory for leaf!");
        return NULL;
    }

    size_t l_count = 0;
    for(size_t i = 0;i<byte_count;i++){
        if(str[i] == '\n'){
            l_count++;
        }
    }
    leaf->line_count = l_count;
    leaf->weight
        = len;
    leaf->byte_count = byte_count;
    leaf->str = malloc(byte_count + 1);
    if (!leaf->str) {
        free(leaf);
        return NULL;
    }
    memcpy(leaf->str, str,byte_count );
    leaf->str[leaf->byte_count] = '\0';
    leaf->left = leaf->right = NULL;
    return leaf;
}

rope_node *make_leaf_paste(const char *str,size_t len){
    if(str == NULL){
        // perror("error making leaf!: string is null");
        return NULL;
    }
    rope_node *leaf = calloc(1,sizeof(rope_node));
    if(leaf == NULL){
        // perror("failed to allocate memory for leaf!");
        return NULL;
    }

    size_t l_count = 0;
    for(size_t i = 0;i<len;i++){
        if(str[i] == '\n'){
            l_count++;
        }
    }
    leaf->line_count = l_count;
    leaf->byte_count = len;
    leaf->str = malloc(len + 1);
    if (!leaf->str) {
        free(leaf);
        return NULL;
    }
    memcpy(leaf->str, str,len );
    leaf->str[leaf->byte_count] = '\0';
    size_t char_len = utf8_char_len(leaf->str);
    leaf->weight
        = char_len;
    leaf->left = leaf->right = NULL;
    return leaf;
}


rope_node *make_leaf_owned(unsigned char *str,size_t len){
    if(str == NULL){
        // perror("failed to make leaf : str is null");
        return NULL;
    }
    rope_node *leaf = calloc(1,sizeof(rope_node));
    if(leaf == NULL){
        // perror("failed to allocate memory for leaf");
        return NULL;
    }
    size_t l_count = 0;
    size_t i = 0;
    while(str[i] != '\0'){
        if(str[i] == '\n'){
            l_count++;
        }
        i++;
    }
    // printf("l=%zu w=%zu b=%zu",l_count,utf8_char_len(str),len);
    leaf->line_count = l_count;
    leaf->weight = utf8_char_len(str);
    leaf->byte_count = len;
    leaf->str = str;
    leaf->str[leaf->byte_count] = '\0';
    leaf->left = leaf->right = NULL;
    return leaf;
}

size_t length(rope_node *node){
    if (node == NULL)return 0;
    if (node->str != NULL ){
        return node->weight ;
    }
    return node->weight + length(node->right);
}

size_t length_byte(rope_node *node){
    if(node == NULL)return 0;
    if(node->str != NULL){
        return node->byte_count;
    }
    return node->byte_count + length_byte(node->right); 
}

size_t lines(rope_node *node){
    if (node == NULL)return 0;
    if (node->str != NULL){
        return node->line_count;
    }
    return node->line_count + lines(node->right);
}                        

rope_node *concat(rope_node *left,rope_node *right){
    if(left == NULL || right == NULL){
        // perror("error concatenating node the left or right node is null");
        return NULL;
    }
    rope_node *node = calloc(1,sizeof(rope_node));
    if(node == NULL){
        // perror("failed to allocate memory for node");
        return NULL;
    }
    node->left = left;
    node->right = right;
    node->str = NULL;
    node->line_count = lines(left);
    node->weight = length(left);
    node->byte_count = length_byte(left);
    return node;
}


void print_rope(rope_node *node) {
    if (node == NULL){
        return;
    };
    if (node->str != NULL && node->left == NULL && node->right == NULL){
        for(size_t i = 0;i<node->byte_count;i++){
            printf("%c",node->str[i]);
        }
    }
    print_rope(node->left);
    print_rope(node->right);
}


void substr(rope_node *node,size_t start,size_t end,rope_node **newsub,mem_for_special *mem){

    if(node == NULL){
        // perror("error getting substring of the node: node is null");
        return;
    }
    
    size_t len = length(node);
    size_t new_end;
    
    if(start > len){
        // perror("start cant be greater than length or less than 0");
        return;
    }
    
    rope_node *l,*r;
    split_rope(node,start ,&l, &r,mem);

    size_t remains = len - start;
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

void split_rope(rope_node *node,size_t pos,rope_node **left,rope_node **right,mem_for_special *mem){
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
            size_t byte_index = utf8_char_byte(node->str, pos);
            unsigned char *left_str = malloc(byte_index+1);
            unsigned char *right_str = malloc((node->byte_count - byte_index) + 1);
            memcpy(left_str, node->str, byte_index);left_str[byte_index] = '\0';
            memcpy(right_str, node->str + byte_index, node->byte_count - byte_index);right_str[node->byte_count - byte_index] = '\0';
            *left = make_leaf_owned(left_str,strlen((char*)left_str));
            *right = make_leaf_owned(right_str,strlen((char*)right_str));
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

size_t leaf_utf8_char_byte(rope_node *node,size_t char_index){
    if(node == NULL)return 0;

    if(node->left == NULL && node->right == NULL){
        return utf8_char_byte(node->str, char_index);
    }

    if(char_index < node->weight){
       return leaf_utf8_char_byte(node->left,char_index);
    }else{
        return node->byte_count  + leaf_utf8_char_byte(node->right, char_index - node->weight );
    }
}

void insert_rope(rope_node *node,size_t pos,char *text,rope_node **root,mem_for_special *mem){
    if(node == NULL){
        // perror("failed to insert rope: rope is null");
        return;
    }
    rope_node *left,*right;
    split_rope(node,pos,&left, &right,mem);
    rope_node *insert_leaf = make_leaf(text);
    add_to_mem(mem, node);
    *root = concat(left, concat(insert_leaf, right));
}

void delete_rope(rope_node *node,size_t pos,rope_node **root,size_t len,mem_for_special *mem,rope_node **deleted){
    if (!node){
        *root = node;
        // perror("error deleting node: node is null");
        return;
    };
    size_t total = length(node);
    if(pos >= total){
        *root = node;
        return;
    }
    size_t right_start = pos + len;
    if(right_start > total)len = total - pos;
    rope_node *left,*right,*temp,*ignore;
    split_rope(node, pos, &left, &temp,mem);
    split_rope(temp, len, &ignore, &right,mem);
    *deleted = ignore;
    add_to_mem(mem, temp);
    *root = concat(left, right);
}

void leaves_count(rope_node *node,size_t *i){
    if (node == NULL)return;
    if (node->str != NULL && node->left == NULL && node->right == NULL){
        *i += 1;
    }
    leaves_count(node->left,i);
    leaves_count(node->right,i);
}



size_t collect_leaves(rope_node *node,rope_node **leaves,size_t index){

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
    size_t count = 0;
    leaves_count(node, &count);
    rope_node **leaves = malloc(sizeof(rope_node*) * count);
    count = 0;
    count = collect_leaves(node, leaves,0);
    if(leaves == NULL)return;
    
    *root = build_balanced_rope(leaves, count);
    free_internal(node,mem);
    free(leaves);
}

rope_node *build_balanced_rope(rope_node **leaves,size_t n){
    if(n <= 0) return NULL;
    if(n == 1) return leaves[0];
    size_t leftn = n/2;
    size_t rightn = n - leftn;
    rope_node *left = build_balanced_rope(leaves, leftn);
    rope_node *right = build_balanced_rope(leaves + leftn, rightn);
    rope_node *parent = malloc(sizeof(rope_node));
    if(parent == NULL){
        // perror("error building balanced rope: memory allocation for parent node failed");
        return NULL;
    }
    parent->str = NULL;
    parent->left = left;
    parent->right = right;
    parent->line_count = left ? lines(left):0;
    parent->weight = left ? length(left):0;
    parent->byte_count = left ? length_byte(left):0;
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

size_t count_depth(rope_node *node){
    if (node == NULL){
        return 0;
    }
    if (node->left == NULL && node->right == NULL)return 1;
    size_t left_depth = count_depth(node->left);
    size_t right_depth = count_depth(node->right);

    return 1 + (left_depth > right_depth ? left_depth:right_depth);
}

size_t fibonacci(size_t n){
    if(n == 0 )return 0;
    if(n == 1 )return 1;
    size_t a = 0,b = 1,c;
    for(size_t i = 2; i <= n; i++){
        c = a + b;
        a = b;
        b = c;
    }
    return b;
}

size_t is_balanced(rope_node *node){
    if (node == NULL) return 2;
    
    size_t rope_length = length(node);
    size_t depth_length = count_depth(node);
    size_t fib_depth = fibonacci(depth_length + 2);
    return rope_length >= fib_depth ? 1:0;
}


const unsigned char *utf8_char_at(const unsigned char *s,size_t i){
    size_t count = 0;
    while(*s){
        if((*s & 0xC0) != 0x80){
            if(count == i)return s;
            count++;
        }
        s++;
    }
    return NULL;
}

const unsigned char* find_char_rope(rope_node *node,size_t i){
    if(node == NULL) return (const unsigned char*)'\0';
    if(i >= length(node)) return (const unsigned char *)'\0';
    static unsigned char buffer[10];
    if(node->right == NULL && node->left == NULL){
        const unsigned char *c =  utf8_char_at(node->str, i);
        size_t bytes = utf8_char_byte(c, 1);
        memcpy(buffer, c, bytes);buffer[bytes] = '\0';
        return buffer;
    }
    
    if(i < node->weight){
      return  find_char_rope(node->left, i );
    }else {

      return find_char_rope(node->right,i - node->weight);
    }
}


void copy_leaves(rope_node *n,unsigned char *buf,size_t *pos){
    if(!n)return;
    if(n->left == NULL && n->right == NULL){
        memcpy(buf + *pos, n->str,n->byte_count);
        *pos += n->byte_count;
        return;
    }
    copy_leaves(n->left,buf,pos);
    copy_leaves(n->right,buf,pos);
}

unsigned char *flatten_to_string(rope_node *node){
    if(node == NULL)return NULL;
    size_t total_len = length_byte(node);
    unsigned char *buffer = malloc(total_len + 1);
    if(!buffer)return NULL;

    size_t offset = 0;

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

rope_node *find_leaf(rope_node *node,size_t i){
    if(node == NULL)return NULL;

    if(node->left == NULL && node->right == NULL){
        return node;
    }

    if(node->byte_count > i){
        return find_leaf(node->left, i);
    }else{
        return find_leaf(node->right, i - node->byte_count);
    }
}

size_t find_start(rope_node *node,size_t i){
    if(node == NULL)return 0;
    if(node->left == NULL && node->right == NULL){
        size_t byte = 0;
        for (size_t len = 1; len <= i; byte++) {
          if (node->str[byte] == '\n')
            len++;
        }
        return byte;
    }
    
    size_t left_lines = node->left ? node->line_count:0; 

    if(left_lines >= i) return find_start(node->left, i);
    else return  node->byte_count + find_start(node->right,i - left_lines);
}


void rope_slice(rope_node *node,size_t line_start,size_t line_end,mem_for_special *mem,rope_node **slice){
    if(node == NULL)return;
    size_t line_total = lines(node->right) + node->line_count;
    if(line_total < line_start || line_total < line_end )return;
    size_t start_offset = find_start(node, line_start);
    size_t end_offset = find_start(node, line_end+line_start);
    substr(node, start_offset, end_offset - start_offset, slice, mem);
    // print_rope(*slice);
}
