#include <stdio.h>
int main () {
    int k=4;
    int *j=&k;
    int **l=&j;
    int i=**l;
    int *h=*l;
    printf("%d", i);
    printf("%d", *h);
}
