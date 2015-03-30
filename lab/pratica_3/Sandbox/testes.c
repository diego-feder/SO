#include<stdio.h>
#include<stdlib.h>

typedef struct node {
    struct node * l;
    struct node *r;
    int i;
} node;

void append(node **q, node *e) {
    if (*q == NULL) {
        e->l = e;
        e->r = e;
        *q = e;
    }
}

int main () {
    node *e = malloc(sizeof(struct node));
    node *q=NULL;
    e->l=NULL;
    e->r=NULL;
    e->i=15;
    append((node **) &q, e);
    printf("%d\t%x\t%x\n", q->i, q->l, q->r);
}
