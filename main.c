#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef const int Int;
typedef const int Bool;


Int
p (void * pointer) {
    Int a = (short)pointer;
    return ((a % 1000) / 4);
}


//
//  SOME HEAP MAP SHIT
//

// double-linked-list for heap map
typedef struct Heap {
    void * pointer;
    int size;
    struct Heap * dep0;
    struct Heap * dep1;
    struct Heap * next;
    struct Heap * prev;
} Heap;


// THE HEAP
Heap * HEAP;
int heapsize = 0;

// nil the HEAP
void
initHeap () {
    HEAP = malloc(sizeof(Heap));
    HEAP -> next = NULL;
}

Heap *
findHeap (Heap * heap, void * pointer) {

    if (pointer == NULL) return NULL;

    if (heap -> next) {
        if (heap -> pointer == pointer)
            return heap;
        else
            return findHeap(heap -> next, pointer);
    } else {
        return NULL;
    }
}

void * 
alloc (Int size, void * dep0Ptr, void * dep1Ptr) {


    void * allocated = malloc(size);

    // extending the HEAP
    Heap * newHeap = malloc(sizeof(Heap));
    newHeap -> pointer = allocated;
    newHeap -> size = size;
    newHeap -> dep0 = findHeap(HEAP, dep0Ptr);
    newHeap -> dep1 = findHeap(HEAP, dep1Ptr);
    newHeap -> next = HEAP;
    newHeap -> prev = NULL;
    HEAP -> prev = newHeap;
    HEAP = newHeap;


    return allocated;
}


void
releaseHeap (Heap * victim) {
    
    if (victim -> dep0)
        releaseHeap(victim -> dep0);
    if (victim -> dep1)
        releaseHeap(victim -> dep1);

    if (victim -> pointer) {
        free((void *)victim -> pointer);
    }


    // delete this heap
    if (victim == HEAP) {
        Heap * next = victim -> next;
        free(victim);
        HEAP = next;
        HEAP -> prev = NULL;        
    } else {
        Heap * next = victim -> next;
        Heap * prev = victim -> prev;
        free(victim);
        prev -> next = next;
        next -> prev = prev;
    }
}

void
release (void * pointer) {
    Heap * victim = findHeap(HEAP, pointer);
    if (victim)
        releaseHeap(victim);
    else
        return;
}

Int
heapSize (Heap * heap) {
    if (heap -> next) {
        return 1 + heapSize(heap -> next);
    } else {
        return 0;
    }
} 



void
printHeap (Heap * heap) {

    if (heap -> next) {
        printf("[%d] %d %d\n", 
            p(findHeap(HEAP, heap -> pointer)), 
            p(heap -> dep0), 
            p(heap -> dep1)
        );
        printHeap(heap -> next);
    }
}


Heap *
copyHeap (Heap * heap) {

    void * data = heap -> pointer;
    Int size = heap -> size;
    Heap * dep0 = heap -> dep0;
    Heap * dep1 = heap -> dep1;
    Heap * dep0_;
    Heap * dep1_;

    if (dep0) {
        dep0_ = copyHeap(dep0);
    }
    
    if (dep1) {
        dep1_ = copyHeap(dep1);
    }

    void * allocated = alloc(size, dep0 ? dep0_ -> pointer : NULL, dep1 ? dep1_ -> pointer : NULL);
    // void * allocated = alloc(size, dep0_ -> pointer, dep1_ -> pointer);
    memcpy(allocated, data, size);


    return findHeap(HEAP, allocated);
}


void *
copy (void * data) {
    Heap * heap = findHeap(HEAP, data);
    return (copyHeap(heap) -> pointer);
}


//
//  HASKELL FTW
//


// type Position = (x, y)
typedef const struct {
    Int x;
    Int y;
} Position;


void
printPosition (Position * p) {
    printf("(%d, %d)\n", p -> x, p -> y);
}


Position *
allocPosition (Int x, Int y) {
    Position position = {x, y};
    void * allocated = alloc(sizeof(Position), NULL, NULL);
    memcpy(allocated, &position, sizeof(Position));
    Position * p = (void *)allocated;
    return p;
}

Bool
equalPosition (Position * a, Position * b) {
    return a -> x == b -> x && a -> y == b -> y;
}

// data List a = Just a | Nothing
typedef const enum MaybeType { Just, Nothing } MaybeType;
typedef const struct Maybe {
    MaybeType type;
    void * data;
} Maybe;


// data List a = Cons a (List a) | Nil
typedef const enum ListType { Nil, Cons } ListType;
typedef const struct List {
    void * data;
    const struct List * cons;
    ListType type;
} List;

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
    List list = { data, cons, type };
    void * allocated = alloc(sizeof(List), (void *)data, (void *)cons);
    memcpy(allocated, &list, sizeof(List));
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

void
printRoute (List * routes) {
    switch (routes -> type) {
        case Nil:
            break;
        case Cons:
            printList(routes -> data, (void *)printPosition);
            printRoute(routes -> cons);
            break;
    }
   
}




// concatenate :: [a] -> [a] -> [a]
// concatenate [] list = list
// concatenate (x:xs) list = x : concatenate xs list
List *
concatenate (List * a, List * b) {
    if (a -> type == Nil) {
        return b;
    } else {
        return cons(a -> data, concatenate(a -> cons, b));
    }
}

// List * 
// filter (List * list, Bool (*f) (void *)) {
//     switch (list -> type) {
//         case Nil:
//             break;
//         case Cons:
//             printList(routes -> data, (void *)printPosition);
//             printRoute(routes -> cons);
//             break;
//     }
// }


// bfs :: Table -> [Route] -> Position -> Maybe (Int, Route)
// bfs _ [] _ = Nothing
// bfs (i, table) (x:xs) target
//     | r == target = Just (i, x)
//     | otherwise = bfs (succ i, r:table) (xs ++ frontier) target
//     where   (r:rs) = x
//             frontier = map attachRoute . filter (flip notElem table) . move $ r
//             attachRoute p = p:x

List *
expandBFS (Position * position) {

}

List *
bfs (List * table, List * routes, Position * target) {
    // route picked to expand
    List * route = routes -> data;
    List * here = route -> data;

    if (equalPosition((Position *)here, target)) {
        printf("HIT!\n");
        return route;
    } else {
        // List * newTable = cons((void *)here, table);
        // List * frontier = 
        // List * newRoutes = concatenate(routes -> cons, frontier);
        // return bfs(newTable, newRoutes, target);
    }
}





int
main () {

    initHeap();

    // Position * target = allocPosition(0, 0);
    // Position * start = allocPosition(0, 0);
    // List * table = nil();
    // List * routes = nil();
    // List * initRoute = nil();
    // initRoute = cons((void *)start, initRoute);
    // routes = cons((void *)initRoute, routes);

    // List * result = bfs(table, routes, target);
    // printRoute(routes);

    // concatenate test
    // List * a = nil();
    // List * b = nil();
    // int i;
    // for (i = 0; i < 4; i++) {
    //     a = cons((void *)i, a);
    // }


    Position * p = allocPosition(0, 0);
    List * a = nil();

    a = cons((void *)p, a);
    printHeap(HEAP);

    List * b = copy((void *)a);
    printf("====================\n");
    printHeap(HEAP);
    printf("====================\n");

    release(b);
    printHeap(HEAP);
    return 0;
}

// int
// main () {
//     return 0;
// }