#include <stdio.h>
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
int main(){
    int num[5] = {1,2,3,4,5};
    printf("%d",num[3]);

}
