#ifndef ROPE_H
#define ROPE_H

#include "stddef.h"

typedef struct rope_node rope_node ;
typedef struct mem_for_special {
    rope_node **arr;
    size_t cap;
    size_t size;
}mem_for_special;

void init_mem_f_s(mem_for_special *mem,size_t initial_cap);
int exists_in_mem(rope_node *node,mem_for_special *mem);
void add_to_mem(mem_for_special *mem,rope_node *node);
rope_node *make_leaf(const char *str);
rope_node *make_leaf_owned(char *str,long len);
long length(rope_node *node);
rope_node *concat(rope_node *left,rope_node *right);
rope_node *substr(rope_node *node,long start,long len);
void print_rope(rope_node *node);
void split_rope(rope_node *node,long pos,rope_node **left,rope_node **right,mem_for_special *mem);
void insert_rope(rope_node *node,long pos,char *text,rope_node **root,mem_for_special *mem);
void delete_rope(rope_node *node,long pos,rope_node **root,long len,mem_for_special *mem);
void leaves_count(rope_node *node,long *i);
int collect_leaves(rope_node *node,rope_node **leaves,int index);
rope_node *build_balanced_rope(rope_node **leaves,long n);
void free_ropes(rope_node *root,mem_for_special *mem);
void free_internal(rope_node *node);
long count_depth(rope_node *node);
unsigned long long fibonacci(long n);
int is_balanced(rope_node *node);
char find_char_rope(rope_node *node,long i);
void copy_leaves(rope_node *n,char *buf,long *pos);
char *flatten_to_string(rope_node *node);
#endif
