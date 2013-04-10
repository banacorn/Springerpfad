#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define False 0
#define True 1
typedef const short Short;
typedef const int Int;
typedef const int Bool;

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
int heapInitialized = False;

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


//////////////////////////////////
////////                  ////////
////////   HASKELL FTW    ////////
////////                  ////////
//////////////////////////////////



Int *
allocInt (Int x) {
    Int integer = {x};
    void * allocated = alloc(sizeof(Int), NULL, NULL);
    memcpy(allocated, &integer, sizeof(Int));
    Int * p = (void *)allocated;
    return p;
}


//
//      type Position = (x, y)
//


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

//
//      data Pair a b = Pair a b
//


typedef const struct Pair {
    void * fst;
    void * snd;
} Pair;

Pair *
allocPair (void * fst, void * snd) {
    Pair pair = {fst, snd};
    void * allocated = alloc(sizeof(Pair), fst, snd);
    memcpy(allocated, &pair, sizeof(Pair));
    Pair * p = (void *)allocated;
    return p;
}
#define allocPair(a, b) allocPair((void *)a, (void *)b)

void
printPair (Pair * pair, void (*showFst) (void *), void (*showSnd) (void *)) {
    printf("Fst ");
    showFst((void *)pair -> fst);
    printf("Snd ");
    showSnd((void *)pair -> snd);
}
#define printPair(a, b, c) printPair(a, (void *)b, (void *)c)

//
//      data Either a b = Left a | Right b
//

typedef const enum EitherType { Left, Right } EitherType;
typedef const struct Either {
    void * data;
    EitherType type;
} Either;

Either *
allocEither (EitherType type, void * data) {
    Either either = {data, type};
    void * allocated = alloc(sizeof(Either), data, NULL);
    memcpy(allocated, &either, sizeof(Either));
    Either * p = (void *)allocated;
    return p;
}
#define allocEither(a, b) allocEither(a, (void *)b)

void
printEither (Either * either, void (*show) (void *)) {
    switch (either -> type) {
        case Left:
            printf("Left ");
            show((void *)either -> data);
            break;
        case Right:
            printf("Right ");
            show((void *)either -> data);
            break;
    }
}
#define printEither(a, b) printEither(a, (void *)b)

//
//      data List a = Just a | Nothing
//

 
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

//
//      data List a = Cons a (List a) | Nil
//

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



typedef const struct Result {
    List * route;
    Int node;
} Result;

Result *
allocResult (Int node, List * route) {
    Result result = { route, node };
    void * allocated = alloc(sizeof(Result), (void *)route, NULL);
    memcpy(allocated, &result, sizeof(Result));
    Result * p = (void *)allocated;
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

Result *
bfs_ (List * table, List * routes, Position * goal) {
    

    table = copy(table);
    routes = copy(routes);
    goal = copy(goal);

    Result * result;
    List * route = routes -> data;
    Position * here = route -> data;
    List * expanded = expand(here);
    List * expandedNotElemOfTable = filterNotElemOfTable(table, expanded);

    Bool goalInFrontier = elem(goal, expandedNotElemOfTable, equalPosition);

    if (equalPosition(here, goal)) {
        result = allocResult(length(table), copy(route));
    } else if (goalInFrontier) {
        List * newRoutes = cons(copy(goal), copy(route));
        result = allocResult(length(table) + 1, newRoutes);   
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


Result *
bfs (Int startX, Int startY, Int goalX, Int goalY) {
    List * table = nil();
    List * routes = cons(cons(allocPosition(startX, startY), nil()), nil());
    Position * goal = allocPosition(goalX, goalY);
    Result * result = bfs_(table, routes, goal);
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
    // printf(":%d", position -> level);
}

void 
printResult (Result * result) {
    printf("%d | ", result -> node);
    printListLn(result -> route, printPosition);
}


List *
mapTagThenAttachRoute (List * route, Int depth, List * list) {
    switch (list -> type) {
        case Nil:
            return nil();
            break;
        case Cons:;
            List * newRoute = cons(allocTaggedPosition(depth + 1, copy(list -> data)), copy(route));
            List * oldResult = mapTagThenAttachRoute(route, depth, list -> cons);
            return cons(newRoute, oldResult);
            break;
    }
}

// dfs :: Table -> [TaggedRoute] -> Limit -> Position -> Maybe (Int, TaggedRoute)
// dfs _ [] _ _ = Nothing
// dfs table (route:xs) limit goal
//     | here == goal || goal `elem` frontier = Just (length table + 1, (goal, 0):route)
//     | depth == limit = dfs newTable (xs)                                     limit goal
//     | otherwise      = dfs newTable (map (attachRoute . tag) frontier ++ xs) limit goal
//     where   (here, depth):rs = route
//             frontier = filter (flip notElem table) . move $ here
//             newTable = if here `notElem` table then here:table else table
//             tag position = (position, depth + 1)
//             attachRoute p = p:route


Either *
dfs (List * table, List * routes, Int limit, Position * goal) {

    table = copy(table);
    routes = copy(routes);
    goal = copy(goal);

    Either * result;

    if (routes -> type == Nil) {
        result = allocEither(Left, allocInt(length(table)));
    } else {


        List * route = routes -> data;
        TaggedPosition * taggedHere = route -> data;
        Position * here = taggedHere -> position;
        Int depth = taggedHere -> level;

        List * expanded = expand(here);
        List * expandedNotElemOfTable = filterNotElemOfTable(table, expanded);
    
        Bool goalInFrontier = elem(goal, expandedNotElemOfTable, equalPosition);

        if (equalPosition(here, goal)) {
            result = allocEither(Right, allocResult(length(table), copy(route)));
        } else if (goalInFrontier) {
            List * newRoutes = cons(allocTaggedPosition(0, copy(goal)), copy(route));
            result = allocEither(Right, allocResult(length(table) + 1, newRoutes));

        } else if (depth == limit) {
            Bool inTable = elem(here, table, equalPosition);
            List * newTable = inTable ? copy(table) : cons(copy(here), copy(table));
            List * newRoutes = copy(routes -> cons);
            result = dfs(newTable, newRoutes, limit, goal);
            release(newTable);
            release(newRoutes);
        } else {
            // printPositionLn(here);
            // printListLn(expanded, printPosition);
            // printListLn(expandedNotElemOfTable, printPosition);
            List * expandedRoutes = mapTagThenAttachRoute(route, depth, expandedNotElemOfTable);
            
            Bool inTable = elem(here, table, equalPosition);
            List * newTable = inTable ? copy(table) : cons(copy(here), copy(table));
            List * newRoutes = concatenate(expandedRoutes, routes -> cons);
            result = dfs(newTable, newRoutes, limit, goal);
            release(newTable);
            release(newRoutes);
            release(expandedRoutes);
        }
        release(expanded);
        release(expandedNotElemOfTable);
    }

    release(table);
    release(routes);
    release(goal);

    return result;
}


// iddfs n x y x' y'
//     | n > 5 = Nothing
//     | otherwise = case dfs [] [[((x, y), 0)]] n (x', y') of
//             Nothing -> iddfs (succ n) x y x' y'
//             Just r -> Just r

Result *
iddfs (Int limit, Int nodeExpanded, Position * start, Position * goal) {

    start = copy(start);
    goal = copy(goal);

    List * table = nil();
    List * routes = cons(cons(allocTaggedPosition(0, copy(start)), nil()), nil());

    Either * attempt = dfs(table, routes, limit, goal);
    Result * result;

    switch (attempt -> type) {
        case Left:;
            Int * n = attempt -> data;
            result = iddfs(limit + 1, *n, start, goal);
            // printf("failed at level %d expanded %d nodes\n", limit, *n + nodeExpanded);
            break;
        case Right:;
            Result * data = attempt -> data;
            result = allocResult(data -> node + nodeExpanded, copy(data -> route));
            // printf("successed at level %d expanded %d nodes\n", limit, data -> node + nodeExpanded);
            break;
    }
    release(start);
    release(goal);
    release(table);
    release(routes);
    release(attempt);
    return result;
}

void
printTaggedReverse (List * list) {
    switch (list -> type) {
        case Nil:
            break;
        case Cons:
            printTaggedReverse(list -> cons);
            printTaggedPosition(list -> data);
            break;
    }
}


///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////
///////////////////////////////////////



typedef const struct Node {
    Position * position;
    Position * parent;
    Int g;
    Int h;
} Node;

Node *
allocNode (Position * position, Position * parent, Int g, Int h) {
    Node node = { position, parent, g, h };
    void * allocated = alloc(sizeof(Node), (void *)position, (void *)parent);
    memcpy(allocated, &node, sizeof(Node));
    Node * p = (void *)allocated;
    return p;
}

void
printNode (Node * node) {
    printPosition(node -> position);
    printPosition(node -> parent);
    printf("%d  ", node -> g + node -> h);
}

Int
heuristic (Position * position, Position * goal) {
    return (Int)floor((abs(goal -> x - position -> x) + abs(goal -> y - position -> y)) / 3);
}

Node *
toNode (Position * position, Position * parent, Int cost, Position * goal) {
    return allocNode(copy(position), copy(parent), cost, heuristic(position, goal));
}

Bool
inQueue (List * queue, Position * position) {
    switch (queue -> type) {
        case Nil:
            return False;
            break;
        case Cons:;

            Node * node = queue -> data;

            if (equalPosition(position, node -> position))
                return True;
            else
                return inQueue(queue -> cons, position);
            break;
    }
}

Int
cost (Node * node) {
    return (node -> g) + (node -> h);
}

// pick :: PQueue -> (PQueue, Node)
// pick queue = (queue \\ [picked], picked)
//     where   picked = minimumBy (comparing cost) queue

Pair *
pick (List * open) {

    Pair * result;
    Node * newNode = open -> data;

    if (length(open) == 1) {
        result = allocPair(nil(), copy(newNode));
    } else {
        Pair * pair = pick(open -> cons);
        List * oldQueue = copy(pair -> fst);
        Node * oldNode = copy(pair -> snd);
        release(pair);
        if (cost(newNode) < cost(oldNode)) {
           result = allocPair(cons(oldNode, oldQueue), copy(newNode));
        } else {
           result = allocPair(cons(copy(newNode), oldQueue), oldNode);
        }
    }

    return result;
}



// findNode p (x:xs)
//     | position x == p   = x
//     | otherwise         = findNode p xs
Node *
findNode (List * queue, Position * position) {

    Node * thisNode = queue -> data;

    if (equalPosition(thisNode -> position, position)) {
        return copy(thisNode);
    } else {
        return findNode(queue -> cons, position);
    }
}


// constructPath :: PQueue -> Position -> Position -> Route
// constructPath queue start goal
//     | start == goal = [start]
//     | otherwise = goal : constructPath queue start goal'
//     where   (_, goal', _, _) = findNode goal queue

List *
constructPath (List * queue, Position * start, Position * goal) {
    List * result;
    if (equalPosition(start, goal)) {
        result = cons(copy(start), nil());
    } else {
        Node * goalNode = findNode(queue, goal);
        // Node * goalParent = findNode(queue, goalNode -> parent);
        result = cons(copy(goal), constructPath(queue, start, goalNode -> parent));
        release(goalNode);
        // release(goalParent);
    }
    return result;
}


// expand :: Position -> Node -> PQueue
// expand goal (position, _, g, h) = map (evalCost g) . move $ position
//     where evalCost g p = (p, position, succ g, heuristic goal p)

List *
expandNode (List * expanded, Node * origin, Position * goal) {
    if (expanded -> type == Nil) {
        return nil();
    } else {
        Node * newNode = toNode(expanded -> data, origin -> position, cost(origin) + 1, goal);
        return cons(newNode, expandNode(expanded -> cons, origin, goal));
    }
}

List *
addNode (List * closed, List * open, Node * expanded) {

    List * result;

    // printf("adding  ");
    // printNode(expanded);
    // printf("\n");

    switch (open -> type) {
        case Nil:
            result = cons(copy(expanded), nil());
            break;
        case Cons:

            if (inQueue(closed, expanded -> position)) {
                result = copy(open);
            } else {

                Node * node = open -> data;
                if (equalPosition(node -> position, expanded -> position)) {
                    // printf("%d %d\n", cost(node), cost(expanded));
                    if (cost(node) > cost(expanded)) {
                        result = cons(copy(expanded), copy(open -> cons));
                    } else {
                        // printf("worse\n");
                        result = copy(open);
                    }
                } else {
                    result = cons(copy(open -> data), addNode(closed, open -> cons, expanded));
                }
                
            }

            break;
    }

    return result;
}

List *
addNodes (List * closed, List * open, List * expanded) {
    switch (expanded -> type) {
        case Nil:
            return copy(open);
            break;
        case Cons:;
            List * newOpen = addNode(closed, open, expanded -> data);
            List * result = addNodes(closed, newOpen, expanded -> cons);
            release(newOpen);
            return result;
            break;
    }
}

// aStar :: PQueue -> PQueue -> Position -> Position -> (Int, Route)
// aStar open closed start goal
//     | start == goal         = (0, [start])
//     | goal `inPQueue` open  = (length closed, constructPath (closed ++ open) start goal)
//     | otherwise             = aStar open' closed' start goal
//     where   (openRest, picked) = pick open
//             open'              = foldl (addNode closed) openRest (expand goal picked)
//             closed'            = picked : closed


Result *
aStar (List * open, List * closed, Position * start, Position * goal) {
    open = copy(open);
    closed = copy(closed);
    start = copy(start);
    goal = copy(goal);

    Result * result;

    if (equalPosition(start, goal)) {
        List * path = cons(copy(start), nil());
        result = allocResult(0, path);
    } else if (inQueue(open, goal)) {
        // printf("in open queue\n");
        List * allSet = concatenate(open, closed);
        List * path = constructPath(allSet, start, goal);
        result = allocResult(length(closed), path);
        release(allSet);

    } else {
        // printf("open\n");
        // printListLn(open, printNode);
        // printf("closed\n");
        // printListLn(closed, printNode);
        // printf("========\n");

        Pair * pickedPair = pick(open);
        List * openRest = pickedPair -> fst;
        Node * picked = pickedPair -> snd;
        List * expanded = expand(picked -> position);
        List * expandedNode = expandNode(expanded, picked, goal);
        // printListLn(expandedNode, printNode);

        List * newOpen = addNodes(closed, openRest, expandedNode);
        List * newClosed = cons(copy(picked), copy(closed));

        // printf("new open\n");
        // printListLn(newOpen, printNode);
        // printf("new closed\n");
        // printListLn(newClosed, printNode);
        // printf("========\n");
        result = aStar(newOpen, newClosed, start, goal);



        release(pickedPair);
        release(expanded);
        release(expandedNode);
        release(newOpen);
        release(newClosed);


    }

    release(open);
    release(closed);
    release(start);
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

    if (!heapInitialized) {
        initHeap();
        heapInitialized = True;
    }

    int node = 0;

    Position * start;
    Position * goal;

    switch (type) {
        case 1:;    
            Result * result = bfs(startX, startY, goalX, goalY);
            printReverse(result -> route);
            node = result -> node;
            release(result);    
            break;
        case 2:;
            start = allocPosition(startX, startY);
            goal = allocPosition(goalX, goalY);
            Result * idresult = iddfs(0, 0, start, goal);
            printTaggedReverse(idresult -> route);
            node = idresult -> node;
            release(start);
            release(goal);
            release(idresult);

            break;
        case 3:;
            start = allocPosition(startX, startY);
            goal = allocPosition(goalX, goalY);

            Node * startNode = allocNode(copy(start), copy(start), 0, heuristic(start, goal));
            List * open = cons(startNode, nil());
            List * closed = nil();
            Result * aStarResult = aStar(open, closed, start, goal);
            printReverse(aStarResult -> route);
            node = aStarResult -> node;
            release(start);
            release(goal);
            release(open);
            release(closed);
            release(aStarResult);

    }
    return node;
}



int
main () {

    initHeap();

    printf("%d\n", theFunction(3, 0, 0, 2, 2));
    printf("%d\n", theFunction(3, 0, 0, 0, 0));

    printf("%d\n", theFunction(2, 0, 0, 2, 2));
    printf("%d\n", theFunction(1, 1, 1, 3, 3));
    printf("%d\n", theFunction(1, 2, 2, 0, 0));
    printf("%d\n", theFunction(1, 2, 2, 4, 4));
    printf("%d\n", theFunction(1, 2, 2, 0, 4));
    printf("%d\n", theFunction(1, 2, 2, 4, 0));


    printf("heap size: %d\n", heapSize(HEAP));





    return 0;
}