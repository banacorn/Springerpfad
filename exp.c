#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef const int Int;  
typedef const int Bool;



//
//
//

typedef const struct {
    Int x;
    Int y;
} Position;



// data List a = Cons a (List a) | Nil
typedef const enum ListType { Nil, Cons } ListType;
typedef const struct List {
    ListType type;
    void * data;
    const struct List * cons;
} List;

void
printPosition (Position * p) {
    printf("(%d, %d)\n", p -> x, p -> y);
}

void
printList_ (List * list, void (*show) (void *)) {
    switch (list -> type) {
        case Nil:
            break;
        case Cons:
            show(list -> data);
            printList_(list -> cons, show);
            break;
    }
}

void
printList (List * list, void (*show) (void *)) {
    printList_(list, show);
}


Int
length (List * list) {
    switch (list -> type) {
        case Nil:
            return 0;
            break;
        case Cons:
            return 1 + length(list -> cons);
            break;
    }
} 


List *
allocList (ListType type, void * data, List * cons) {
    List list = { type, data, cons };
    void * allocated = alloc(sizeof(List), (void *)data, (void *)cons);
    memcpy(allocated, &list , sizeof(List));
    List * p = (void *)allocated;
    return p;
}

List *
nil () {
    return allocList(Nil, NULL, NULL);
}

List *
cons (void * data, List * b) {
    List * n = allocList(Cons, data, b);
    return n;
}


int main () {

    initHeap();

    List * list = nil();
    int i, j;
    for (i = 0; i < 5; i++) {
        List * inner = nil();
        for (j = 0; j < 5; j++)
        {
            Position * p = alloc(sizeof(Position), NULL, NULL);
            inner = cons((void *)p, inner);
        }
        list = cons((void *)inner, list);
    }

    printHeap(HEAP);
    printf("%d\n", heapSize(HEAP));


    release((void *)list);

    printf("========\n");
    printHeap(HEAP);

    return 0;
}