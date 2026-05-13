#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rope.h"

static char* xstrdup(const char *s){
  size_t n = strlen(s);
  char *p = malloc(n+1);
  memcpy(p, s, n+1);
  return p;
}

static char* ref_insert(char *s, size_t pos, const char *ins){
  size_t sl=strlen(s), il=strlen(ins);
  if(pos>sl) pos=sl;
  char *out = malloc(sl+il+1);
  memcpy(out, s, pos);
  memcpy(out+pos, ins, il);
  memcpy(out+pos+il, s+pos, sl-pos);
  out[sl+il]='\0';
  free(s);
  return out;
}

static char* ref_delete(char *s, size_t pos, size_t len){
  size_t sl=strlen(s);
  if(pos>sl) pos=sl;
  if(pos+len>sl) len=sl-pos;
  char *out = malloc(sl-len+1);
  memcpy(out, s, pos);
  memcpy(out+pos, s+pos+len, sl-(pos+len));
  out[sl-len]='\0';
  free(s);
  return out;
}

static void basic_tests(){
  mem_for_special mem; init_mem_f_s(&mem, 64);
  rope_node *root = make_leaf("");

  rope_append(&root, "hello");
  unsigned char *flat = flatten_to_string(root);
  if(strcmp((char*)flat, "hello")) { puts("basic fail"); exit(1); }
  free(flat);

  insert_rope(root, 0, "A", &root, &mem);
  flat = flatten_to_string(root);
  if(strcmp((char*)flat, "Ahello")) { puts("basic fail"); exit(1); }
  free(flat);

  rope_node *deleted = NULL;
  delete_rope(root, 2, &root, 2, &mem, &deleted);
  flat = flatten_to_string(root);
  free(flat);

  free_ropes(root, &mem);
  free_ropes(deleted, &mem);
  free_mem(&mem);
}

static void fuzz_ascii(unsigned seed){
  srand(seed);
  mem_for_special mem; init_mem_f_s(&mem, 2048);
  rope_node *root = make_leaf("");
  char *ref = xstrdup("");

  const char alphabet[]="abcdefghijklmnopqrstuvwxyz";

  for(int step=0; step<800; step++){
    int op = rand()%3;
    size_t len = strlen(ref);

    if(op==0){
      size_t pos = (len==0)?0:(size_t)(rand()%(len+1));
      int ilen = 1 + rand()%8;
      char buf[16];
      for(int i=0;i<ilen;i++) buf[i]=alphabet[rand()%26];
      buf[ilen]='\0';

      insert_rope(root, pos, buf, &root, &mem);
      ref = ref_insert(ref, pos, buf);
    } else if(op==1){
      if(len==0) continue;
      size_t pos = (size_t)(rand()%len);
      size_t dlen = 1 + (size_t)(rand()%8);

      rope_node *deleted = NULL;
      delete_rope(root, pos, &root, dlen, &mem, &deleted);
      ref = ref_delete(ref, pos, dlen);
    } else {
      int ilen = 1 + rand()%8;
      char buf[16];
      for(int i=0;i<ilen;i++) buf[i]=alphabet[rand()%26];
      buf[ilen]='\0';

      rope_append(&root, buf);
      ref = ref_insert(ref, strlen(ref), buf);
    }

    unsigned char *flat = flatten_to_string(root);
    if(strcmp((char*)flat, ref)!=0){
      printf("Mismatch at step %d\n", step);
      free(flat);
      exit(1);
    }
    free(flat);
  }

  free(ref);
  free_ropes(root, &mem);
  free_mem(&mem);
}

int main(){
  basic_tests();
  for(unsigned s=1; s<=5; s++) fuzz_ascii(12345u*s);
  puts("OK (but check LeakSanitizer output)");
  return 0;
}
