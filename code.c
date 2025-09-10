#include <stdio.h>
int main(){
    long s = 100;
    long *t = &s;
    long **tt = &t;
    long ***ttt = &tt;
    long ****tttt = &ttt;
    printf("%zu",****(tttt));
}
