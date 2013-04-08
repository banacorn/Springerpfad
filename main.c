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
    
    // release dependent heaps
    if (victim -> dep0)
        releaseHeap(victim -> dep0);
    if (victim -> dep1)
        releaseHeap(victim -> dep1);

    // release data held
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

        void * dep0Data = (void *)((long *)heap -> pointer)[0];
        void * dep1Data = (void *)((long *)heap -> pointer)[1];

        printf("[%p %d]\t%p %d\t%p %d\n", 
            heap, 
            heap -> size,
            heap -> dep0,   
            heap -> dep0 ? heap -> dep0 -> size : 0,
            heap -> dep1,
            heap -> dep1 ? heap -> dep1 -> size : 0
        );
        if (heap -> dep0 && heap -> dep0 != findHeap(HEAP, dep0Data))
            printf("CORRUPTED %p %d -> %p %d\n", 
                heap -> dep0, 
                heap -> dep0 -> size, 
                findHeap(HEAP, dep0Data), 
                findHeap(HEAP, dep0Data) ? findHeap(HEAP, dep0Data) -> size : 0
            );
        if (heap -> dep1 && heap -> dep1 != findHeap(HEAP, dep1Data))
            printf("CORRUPTED %p %d -> %p %d\n", 
                heap -> dep1, 
                heap -> dep1 -> size, 
                findHeap(HEAP, dep1Data), 
                findHeap(HEAP, dep1Data) ? findHeap(HEAP, dep1Data) -> size : 0
            );
        printHeap(heap -> next);
    }
}

void
dump () {
    printHeap(HEAP);
}

void
traceHeap (Heap * heap) {
    void * dep0Data = (void *)((long *)heap -> pointer)[0];
    void * dep1Data = (void *)((long *)heap -> pointer)[1];

    printf("[%p %d]\t%p\t%p\n", 
        heap,
        heap -> size,
        heap -> dep0,   
        heap -> dep1
    );
    if (heap -> dep0 && heap -> dep0 != findHeap(HEAP, dep0Data))
        printf("CORRUPTED %p %d -> %p %d\n", 
            heap -> dep0, 
            heap -> dep0 -> size, 
            findHeap(HEAP, dep0Data), 
            findHeap(HEAP, dep0Data) ? findHeap(HEAP, dep0Data) -> size : 0
        );
    if (heap -> dep1 && heap -> dep1 != findHeap(HEAP, dep1Data))
        printf("CORRUPTED %p %d -> %p %d\n", 
            heap -> dep1, 
            heap -> dep1 -> size, 
            findHeap(HEAP, dep1Data), 
            findHeap(HEAP, dep1Data) ? findHeap(HEAP, dep1Data) -> size : 0
        );

    if (heap -> dep0)
        traceHeap(heap -> dep0);
    if (heap -> dep1)
        traceHeap(heap -> dep1);
}

void trace (void * data) {
    Heap * heap = findHeap(HEAP, data);
    traceHeap(heap);
}
#define trace(a) trace((void *)a)

Heap *
copyHeap (Heap * heap) {

    Heap * copiedDep0 = NULL;
    Heap * copiedDep1 = NULL;


    // copy dependent heaps, if there's any
    if (heap -> dep0) {
        copiedDep0 = copyHeap(heap -> dep0);
    }
    if (heap -> dep1) {
        copiedDep1 = copyHeap(heap -> dep1);
    }



    void * newAllocated = alloc(heap -> size, 
        copiedDep0 ? copiedDep0 -> pointer : NULL, 
        copiedDep1 ? copiedDep1 -> pointer : NULL);
    memcpy(newAllocated, heap -> pointer, heap -> size);

    if (heap -> dep0)
        ((long *)newAllocated)[0] = (long)copiedDep0 -> pointer;
    if (heap -> dep1)
        ((long *)newAllocated)[1] = (long)copiedDep1 -> pointer;

    void * dep0Data = (void *)((long *)findHeap(HEAP, newAllocated) -> pointer)[0];
    void * dep1Data = (void *)((long *)findHeap(HEAP, newAllocated) -> pointer)[1];
    
        // if (copiedDep0 && copiedDep0 != findHeap(HEAP, dep0Data))
        //     printf("CORRUPTED %p -> %p\n", heap -> dep0, findHeap(HEAP, dep0Data));
        // if (copiedDep1 && copiedDep1 != findHeap(HEAP, dep1Data))
        //     printf("CORRUPTED %p -> %p\n", heap -> dep1, findHeap(HEAP, dep0Data));


    return findHeap(HEAP, newAllocated);
}


void *
copy (void * data) {
    Heap * heap = findHeap(HEAP, data);
    return (copyHeap(heap) -> pointer);
}
#define copy(a) copy((void *)a)


///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


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
    printf("(%d, %d) ", p -> x, p -> y);
}

void
printPositionLn (Position * p) {
    printf("(%d, %d) \n", p -> x, p -> y);
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
    void * data;
    MaybeType type;
} Maybe;

Maybe *
allocMaybe (MaybeType type, void * data) {
    Maybe maybe = {data, type};
    void * allocated = alloc(sizeof(Maybe), data, NULL);
    memcpy(allocated, &maybe, sizeof(Maybe));
    Maybe * p = (void *)allocated;
    return p;
}
#define allocMaybe(a, b) allocMaybe(a, (void *)b)

void
printMaybe (Maybe * maybe, void (*show) (void *)) {
    switch (maybe -> type) {
        case Nothing:
            printf("Nothing\n");
            break;
        case Just:
            printf("Just ");
            show((void *)maybe -> data);
            break;
    }
}
#define printMaybe(a, b) printMaybe(a, (void *)b)

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



///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////





typedef const struct BFSResult {
    List * route;
    Int node;
} BFSResult;

BFSResult *
allocBFSResult (Int node, List * route) {
    BFSResult bfsResult = { route, node };
    void * allocated = alloc(sizeof(BFSResult), (void *)route, NULL);
    memcpy(allocated, &bfsResult, sizeof(BFSResult));
    BFSResult * p = (void *)allocated;
    return p;
}

Bool
onBoard (Position * position) {
    return position -> x >= 0 
        && position -> x < 8 
        && position -> y >= 0 
        && position -> y < 8;
}

List *
mapAttachRoute (List * route, List * list) {
    switch (list -> type) {
        case Nil:
            return nil();
            break;
        case Cons:;
            List * newRoute = cons(copy(list -> data), copy(route));
            List * oldResult = mapAttachRoute(route, list -> cons);
            return cons(newRoute, oldResult);
            break;
    }
}

List *
filterNotElemOfTable (List * table, List * list) {
    switch (list -> type) {
        case Nil:
            return nil();
            break;
        case Cons:;
            Bool notElem = !elem(list -> data, table, equalPosition);
            if (notElem) {
                return cons(copy(list -> data), filterNotElemOfTable(table, list -> cons));
            } else {
                return filterNotElemOfTable(table, list -> cons);
            }
            break;
    }
}

List * 
expand (Position * position) {
    Int x = position -> x;
    Int y = position -> y;
    List * list = nil();

    list = cons(allocPosition(x + 1, y + 2), list);
    list = cons(allocPosition(x + 2, y + 1), list);
    list = cons(allocPosition(x + 2, y - 1), list);
    list = cons(allocPosition(x + 1, y - 2), list);
    list = cons(allocPosition(x - 1, y - 2), list);
    list = cons(allocPosition(x - 2, y - 1), list);
    list = cons(allocPosition(x - 2, y + 1), list);
    list = cons(allocPosition(x - 1, y + 2), list);
    List * filtered = filter(list, onBoard);
    release(list);
    return filtered;
}

// bfs :: Table -> [Route] -> Position -> Maybe (Int, Route, Table)
// bfs _ [] _ = Nothing
// bfs table (x:xs) goal
//     | r == goal || goal `elem` frontier = Just (length table', goal:x, table')
//     | otherwise = bfs newTable (xs ++ map attachRoute frontier) goal
//     where   (r:rs) = x
//             frontier = filter (flip notElem table) . move $ r
//             table' = r:table 
//             newTable = if r `notElem` table then r:table else table

BFSResult *
bfs_ (List * table, List * routes, Position * goal) {
    

    table = copy(table);
    routes = copy(routes);
    goal = copy(goal);

    BFSResult * result;
    List * route = routes -> data;
    Position * here = route -> data;
    List * expanded = expand(here);
    List * expandedNotElemOfTable = filterNotElemOfTable(table, expanded);

    Bool goalInFrontier = elem(goal, expandedNotElemOfTable, equalPosition);

    if (equalPosition(here, goal) || goalInFrontier) {
        List * newRoutes = cons(copy(goal), copy(route));
        result = allocBFSResult(length(table) + 1, newRoutes);   
    } else {
        List * expandedRoutes = mapAttachRoute(route, expandedNotElemOfTable);
        List * newRoutes = concatenate(routes -> cons, expandedRoutes);
        Bool inTable = elem(here, table, equalPosition);
        List * newTable = inTable ? copy(table) : cons(copy(here), copy(table));
        result = bfs_(newTable, newRoutes, goal);
        release(newRoutes);
        release(newTable);
        release(expandedRoutes);
    }


    release(expanded);
    release(expandedNotElemOfTable);
    release(table);
    release(routes);
    release(goal);

    return result;
}


BFSResult *
bfs (Int startX, Int startY, Int goalX, Int goalY) {
    List * table = nil();
    List * routes = cons(cons(allocPosition(startX, startY), nil()), nil());
    Position * goal = allocPosition(goalX, goalY);
    BFSResult * result = bfs_(table, routes, goal);
    release(goal);
    release(table);
    release(routes);

    return result;
}


void
printReverse (List * list) {
    switch (list -> type) {
        case Nil:
            break;
        case Cons:
            printReverse(list -> cons);
            printPosition(list -> data);
            break;
    }
}


///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


typedef const struct TaggedPosition {
    Position * position;
    Int level;
} TaggedPosition;


TaggedPosition *
allocTaggedPosition (Int level, Position * position) {
    TaggedPosition taggedPosition = { position, level };
    void * allocated = alloc(sizeof(TaggedPosition), (void *)position, NULL);
    memcpy(allocated, &taggedPosition, sizeof(TaggedPosition));
    TaggedPosition * p = (void *)allocated;
    return p;
}

void
printTaggedPosition (TaggedPosition * position) {
    printPosition(position -> position);
    printf(":%d", position -> level);
}

typedef const struct DFSResult {
    List * route;
    Int node;
} DFSResult;

DFSResult *
allocDFSResult (Int node, List * route) {
    DFSResult dfsResult = { route, node };
    void * allocated = alloc(sizeof(DFSResult), (void *)route, NULL);
    memcpy(allocated, &dfsResult, sizeof(DFSResult));
    DFSResult * p = (void *)allocated;
    return p;
}

void 
printDFSResult (DFSResult * result) {
    printf("%d | ", result -> node);
    printListLn(result -> route, printTaggedPosition);
}


// dfs :: Table -> [TaggedRoute] -> Limit -> Position -> Maybe (Int, TaggedRoute)
// dfs _ [] _ _ = Nothing
// dfs table (route:xs) limit goal
//     | here == goal = Just (length table, route)
//     | depth == limit = dfs (here:table) (xs) limit goal
//     | otherwise = dfs newTable (frontier ++ xs) limit goal
//     where   (here, depth):rs = route
//             frontier = map (attachRoute . tag) . filter (flip notElem table) . expand $ here
//             newTable = if here `notElem` table then here:table else table
//             tag position = (position, depth + 1)
//             attachRoute p = p:route

// List *
// mapDFSFrontier 
// switch (list -> type) {
//         case Nil:
//             return nil();
//             break;
//         case Cons:;
//             Bool notElem = !elem(list -> data, table, equalPosition);
//             if (notElem) {
//                 List * newRoute = cons(copy(list -> data), copy(route));
//                 List * oldResult = mapFrontier(table, route, list -> cons);
//                 List * result = cons(newRoute, oldResult);
//                 return result;
//             } else {
//                 return mapFrontier(table, route, list -> cons);
//             }
//             break;
//     }

// List * 
// expandDFS (List * table, List * route, Position * position) {
//     table = copy(table);
//     route = copy(route);
//     List * expanded = expand(position);
//     List * result = mapDFSFrontier(table, route, expanded);
//     release(expanded);
//     release(table);
//     release(route);
//     return result;
// }

Maybe *
dfs (List * table, List * routes, Int limit, Position * goal) {

    table = copy(table);
    routes = copy(routes);
    goal = copy(goal);

    Maybe * result;

    if (routes -> type == Nil) {
        result = allocMaybe(Nothing, NULL);
    } else {


        List * route = routes -> data;
        TaggedPosition * taggedHere = route -> data;
        Position * here = taggedHere -> position;
        Int depth = taggedHere -> level;

        if (equalPosition(here, goal)) {
            result = allocMaybe(Just, allocDFSResult(length(table), copy(route)));
        } else if (depth == limit) {
            List * newTable = cons(copy(here), copy(table));
            List * newRoutes = copy(routes -> cons);
            result = dfs(newTable, newRoutes, limit, goal);
            release(newTable);
            release(newRoutes);
        } else {
            result = allocMaybe(Nothing, NULL);
        }
    }

    release(table);
    release(routes);
    release(goal);

    return result;
}


///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////


int
theFunction (Int type, Int startX, Int startY, Int goalX, Int goalY) {
    int node = 0;
    switch (type) {
        case 1:;    
            BFSResult * result = bfs(startX, startY, goalX, goalY);
            printReverse(result -> route);
            node = result -> node;
            release(result);    
            break;
    }
    return node;
}



int
main () {

    initHeap();

    printf("%d\n", theFunction(1, 0, 0, 1, 2));
    printf("%d\n", theFunction(1, 0, 0, 2, 1));
    printf("%d\n", theFunction(1, 0, 0, 2, 2));
    // printf("%d\n", theFunction(1, 1, 1, 3, 3));
    // printf("%d\n", theFunction(1, 2, 2, 0, 0));
    // printf("%d\n", theFunction(1, 2, 2, 4, 4));
    // printf("%d\n", theFunction(1, 2, 2, 0, 4));
    // printf("%d\n", theFunction(1, 2, 2, 4, 0));

    // List * table = nil();
    // List * routes = cons(cons(allocTaggedPosition(0, allocPosition(0, 0)), nil()), nil());
    // Position * goal = allocPosition(0, 0);
    // Maybe * result = dfs(table, routes, 100, goal);

    // printMaybe(result, printDFSResult);


    // release(table);
    // release(routes);
    // release(goal);
    // release(result);



    printf("heap size: %d\n", heapSize(HEAP));





    return 0;
}