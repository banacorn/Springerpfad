#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define False 0
#define True 1
typedef const short Short;
typedef const int Int;
typedef const int Bool;


// Short
// p (void * pointer) {
//     short a = (short)pointer;
//     return ((a % 1000) / 4);
//     // return pointer;
// }
// #define p(a) p((void *)a)

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
#define findHeap(a, b) findHeap(a, (void *)b)

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
#define release(a) release((void *)a)


void
clearHeap (Heap * heap) {
    if (heap -> next) {
        // releaseHeap(heap);
        // if (heap -> pointer)
            // free(heap -> pointer);
        Heap * next = heap -> next;
        free(heap);
        clearHeap(next);
    }
}

Int
heapSize (Heap * heap) {
    if (heap -> next) {
        return 1 + heapSize(heap -> next);
    } else {
        return 0;
    }
} 

unsigned int
p (void * pointer) {
    long ptr = (long) pointer;
    return ((unsigned int)ptr % 1000) / 8;
}
#define p(a) p((void *)a)

void
printHeap (Heap * heap) {

    if (heap -> next) {
        printf("[%u]\t%u\t(%u)\t%u\t(%u)\n", 
            p(findHeap(HEAP, heap -> pointer)), 
            p(heap -> dep0),   
            heap -> dep0 ? p(findHeap(HEAP, ((long *)heap -> pointer)[0])) : NULL,
            p(heap -> dep1),
            heap -> dep1 ? p(findHeap(HEAP, ((long *)heap -> pointer)[1])) : NULL
        );
        printHeap(heap -> next);
    }
}


Heap *
copyHeap (Heap * heap) {


    printf("*** copying %u of size %d ***\n", p(heap), heap -> size);

    void * data = heap -> pointer;
    Int size = heap -> size;
    Heap * dep0 = heap -> dep0;
    Heap * dep1 = heap -> dep1;
    Heap * dep0_;
    Heap * dep1_;


    if (dep0) {
        dep0_ = copyHeap(dep0);
        printf("     dep0 %u -> %u ***\n", p(dep0), p(dep0_));
        printPositionLn(dep0 -> pointer);
        printPositionLn(dep0_ -> pointer);

    }

    if (dep1) {
        dep1_ = copyHeap(dep1);
        printf("     dep0 %u -> %u ***\n", p(dep1), p(dep1_));
    }

    void * allocated = alloc(size, 
        dep0 ? dep0_ -> pointer : NULL, 
        dep1 ? dep1_ -> pointer : NULL
    );
    memcpy(allocated, data, size);

    // printf("%p\n", allocated);
    // printf("%p\n", allocated + 1);
    // memcpy(allocated, data, 4);

    if (dep0)
        ((long *)allocated)[0] = (long)dep0_ -> pointer;
    if (dep1)
        ((long *)allocated)[1] = (long)dep1_ -> pointer;

    return findHeap(HEAP, allocated);
}


void *
copy (void * data) {
    Heap * heap = findHeap(HEAP, data);



    Heap * copied = (copyHeap(heap) -> pointer);
    // if (heap -> size == 8) {
    //     printf("BINGO ");
    //     printPosition(heap -> pointer);
    //     printPosition(copied);
    //     // printPosition(findHeap(HEAP, allocated) -> pointer);
    //     printf("\n");
    // }
    return copied;
}
#define copy(a) copy((void *)a)


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
    printf("(%d, %d) \t", p -> x, p -> y);
}

void
printPositionLn (Position * p) {
    printf("(%d, %d) \t\n", p -> x, p -> y);
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
#define printList(a, b) printList(a, (void *)b)

void
printListLn (List * list, void (*show) (void *)) {
    printList_(list, show);
    printf("\n");
}
#define printListLn(a, b) printListLn(a, (void *)b)

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
    return allocList(Cons, data, b);
}
#define cons(a, b) cons((void *)a, b)


void
printRoutes (List * routes) {
    switch (routes -> type) {
        case Nil:
            break;
        case Cons:
            printList(routes -> data, (void *)printPosition);
            printf("\n");
            printRoutes(routes -> cons);
            break;
    }
   
}




// concatenate :: [a] -> [a] -> [a]
// concatenate [] list = list
// concatenate (x:xs) list = x : concatenate xs list
List *
concatenate (List * a, List * b) {
    if (a -> type == Nil) {
        return copy(b);
    } else {
        return cons(copy(a -> data), concatenate(a -> cons, b));
    }
}

List * 
filter (List * list, Bool (*f) (void *)) {
    switch (list -> type) {
        case Nil:
            return nil();
            break;
        case Cons:
            if (f(list -> data)) {
                return cons(copy(list -> data), filter(list -> cons, f));
            } else {
                return filter(list -> cons, f);
            }
            break;
    }
}
#define filter(a, f) filter(a, (void *)f)

List * 
map (List * list, void * (*f) (void *)) {
    switch (list -> type) {
        case Nil:
            return nil();
            break;
        case Cons:
            return cons(copy(f(list -> data)), map(list -> cons, (void *)f));
            break;
    }
}
#define map(a, f) map(a, (void *)f)




Bool
elem (void * element, List * list, Bool (*eq) (void *, void *)) {
    switch (list -> type) {
        case Nil:
            return False;
            break;
        case Cons:
            if (eq(list -> data, element)) {
                return True;
            } else {
                return False + elem(element, list -> cons, eq);
            }
            break;
    }
}
#define elem(a, l, eq) elem((void *)a, l, (void *)eq);

Bool
onBoard (Position * position) {
    return position -> x >= 0 
        && position -> x < 8 
        && position -> y >= 0 
        && position -> y < 8;
}

List *
mapFrontier (List * table, List * route, List * list) {
    switch (list -> type) {
        case Nil:
            return nil();
            break;
        case Cons:;
            Bool notElem = !elem(list -> data, table, equalPosition);
            if (notElem) {
                List * newRoute = cons(copy(list -> data), copy(route));
                List * oldResult = mapFrontier(table, route, list -> cons);
                List * result = cons(newRoute, oldResult);
                return result;
            } else {
                return mapFrontier(table, route, list -> cons);
            }
            break;
    }
}

List * 
move (Position * position) {
    Int x = position -> x;
    Int y = position -> y;
    List * list = nil();

    list = cons(allocPosition(x + 1, y + 2), list);
    list = cons(allocPosition(x + 1, y - 2), list);
    list = cons(allocPosition(x - 1, y + 2), list);
    list = cons(allocPosition(x - 1, y - 2), list);
    list = cons(allocPosition(x + 2, y + 1), list);
    list = cons(allocPosition(x + 2, y - 1), list);
    list = cons(allocPosition(x - 2, y + 1), list);
    list = cons(allocPosition(x - 2, y - 1), list);
    List * filtered = filter(list, onBoard);
    release(list);
    return filtered;
}


List * 
expandBFS (List * table, List * route, Position * position) {
        // int a = heapSize(HEAP);
    List * table_ = copy(table);
    List * route_ = copy(route);
    List * moved = move(position);
    List * result = mapFrontier(table_, route_, moved);
    release(moved);
    release(table_);
    release(route_);
        // printf("%d -> %d\n", a, heapSize(HEAP));
    return result;
}


// bfs :: Table -> [Route] -> Position -> Maybe (Int, Route)
// bfs _ [] _ = Nothing
// bfs (i, table) (x:xs) target
//     | r == target = Just (i, x)
//     | otherwise = bfs (succ i, r:table) (xs ++ frontier) target
//     where   (r:rs) = x
//             frontier = map attachRoute . filter (flip notElem table) . move $ r
//             attachRoute p = p:x


List *
bfs_ (List * table, List * routes, Position * target) {

    // route picked to expand
    List * route = routes -> data;
    Position * here = route -> data;

    if (equalPosition(here, target)) {    
        printf("HIT\n");     
        return route;
    } else {

        int a = heapSize(HEAP);
        // printf("GO\n");
        // printf("table ");
        // printListLn(table, printPosition);
        // printf("routes \n");
        // printRoutes(routes);
        // printf("here ");
        // printPositionLn(here);

        List * frontier = expandBFS(table, route, here);

        List * table = cons(copy(here), table);
        List * newRoutes = concatenate(routes -> cons, frontier);
        List * result = bfs_(copy(table), newRoutes, target);

        // printRoutes(frontier);
        // release(frontier);
        // printRoutes(f);


        printf("=====\n");
        release(newRoutes);
        release(table);
        // printf("%d -> %d\n", a, heapSize(HEAP));

        return result;     
    } 
}

List *
bfs (List * table, List * routes, Position * target) {
    List * table_ = copy(table);
    List * routes_ = copy(routes);
    Position * target_ = copy(target);
    List * result = bfs_(table_, routes_, target_);
    release(target_);
    release(table_);
    release(routes_);
    return result;
}





int
main () {

    initHeap();
    // Position * target = allocPosition(2, 1);
    // // Position * start = allocPosition(0, 0);
    // List * table = nil();
    // List * initRoute = cons(allocPosition(0, 0), nil());
    // List * routes = cons(initRoute, nil());

    // // printHeap(HEAP);
    // // printf("=======\n");
    // List * a = bfs(table, routes, target);
    // printListLn(a, printPosition);

    // // printf("====================\n");

    // release(target);
    // release(target);
    // release(table);
    // release(routes);
    // release(a);
    // printf("%d\n", heapSize(HEAP));

    // printHeap(HEAP);


    // Position * start = allocPosition(3, 3);
    // List * table = nil();
    // List * initRoute = cons(allocPosition(0, 0), nil());
    // List * initRoute_ = copy(initRoute);
    // release(initRoute);

    // List * frontier = expandBFS(table, initRoute_, start);
    // printRoutes(frontier);

    // // release(start);
    // // release(table);
    // printf("%d\n", heapSize(HEAP));

    // // release(frontier);

    // printf("%d\n", heapSize(HEAP));

    List * initRoute = cons(allocPosition(0, 0), nil());

    printf("=== allocated ==\n");
    printHeap(HEAP);

    List * initRoute_ = copy(initRoute);


    printf("=== copied ==\n");
    printHeap(HEAP);

    printListLn(initRoute, printPosition);
    release(initRoute);
    printf("=== copy released ==\n");
    printHeap(HEAP);

    printListLn(initRoute_, printPosition);




    // List * a = nil();
    // List * b = nil();
    // List * d = nil();
    // List * e = nil();
    // int i;
    // for (i = 0; i < 5; i++) {
    //     Position * p = allocPosition(i, i);
    //     a = cons(p, a);
    //     b = cons(copy(p), b);

    //     d = cons(copy(a), d);
    //     d = cons(copy(b), d);

    //     e = cons(copy(d), e);
    // }

    // List * c = concatenate(a, b);


    // printf("%d\n", heapSize(HEAP));


    // // printListLn(a, printPosition);
    // release(a);
    // release(b);
    // release(c);
    // release(d);
    // release(e);
    // printf("%d\n", heapSize(HEAP));

    return 0;
}

// int
// main () {
//     return 0;
// }