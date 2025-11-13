#ifndef ROPE_H
#define ROPE_H
#include "stddef.h"
#define CHUNK_SIZE 256

typedef struct rope_node{
    size_t weight;
    unsigned char *str;
    size_t line_count;
    size_t byte_count;
    int deleted;
    struct rope_node *left;
    struct rope_node *right;
}rope_node;

typedef struct mem_for_special {
    rope_node **arr;
    size_t cap;
    size_t size;
}mem_for_special;

size_t utf8_char_len(const unsigned char *string);
void free_mem(mem_for_special *mem);
size_t lines(rope_node *node);
void init_mem_f_s(mem_for_special *mem,size_t initial_cap);
int exists_in_mem(rope_node *node,mem_for_special *mem);
void add_to_mem(mem_for_special *mem,rope_node *node);
rope_node *make_leaf(const char *str);
rope_node *make_leaf_owned(unsigned char *str,size_t len);
size_t length(rope_node *node);
rope_node *concat(rope_node *left,rope_node *right);
void rebalance(rope_node *node,rope_node **root,mem_for_special *mem);
void print_rope(rope_node *node);
size_t utf8_char_byte(const unsigned char *string,size_t char_index);
void substr(rope_node *node,size_t start,size_t end,rope_node **newsub,mem_for_special *mem);
void split_rope(rope_node *node,size_t pos,rope_node **left,rope_node **right,mem_for_special *mem);
void insert_rope(rope_node *node,size_t pos,char *text,rope_node **root,mem_for_special *mem);
void delete_rope(rope_node *node,size_t pos,rope_node **root,size_t len,mem_for_special *mem,rope_node **deleted);
void leaves_count(rope_node *node,size_t *i);
size_t collect_leaves(rope_node *node,rope_node **leaves,size_t index);
rope_node *build_balanced_rope(rope_node **leaves,size_t n);
void rope_append(rope_node **root,const char *str);
void free_ropes(rope_node *root,mem_for_special *mem);
void free_internal(rope_node *node,mem_for_special *mem);
size_t count_depth(rope_node *node);
size_t fibonacci(size_t n);
size_t is_balanced(rope_node *node);
const unsigned char *find_char_rope(rope_node *node,size_t i);
const unsigned char *utf8_char_at(const unsigned char *s,size_t i);
void copy_leaves(rope_node *n,unsigned char *buf,size_t *pos);
unsigned char *flatten_to_string(rope_node *node);
#endif
