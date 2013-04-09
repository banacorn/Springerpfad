
import Data.List (nub, (\\), sortBy)
import Data.Ord (comparing)

type Depth = Int
type Limit = Int
type Position = (Int, Int)
type Table = [Position]
type Route = [Position]
type TaggedRoute = [(Position, Int)]

bfs :: Table -> [Route] -> Position -> Maybe (Int, Route)
bfs _ [] _ = Nothing
bfs table (x:xs) goal
    | r == goal || goal `elem` frontier = Just (length table + 1, goal:x)
    | otherwise = bfs newTable (xs ++ map attachRoute frontier) goal
    where   (r:rs) = x
            frontier = filter (flip notElem table) . move $ r
            newTable = if r `notElem` table then r:table else table
            attachRoute p = p:x

a = bfs [] [[(0, 0)]] (1, 2)
b = bfs [] [[(0, 0)]] (2, 1)

erste x y x' y' = bfs [] [[(x, y)]] (x', y')


r = nub $ return (3, 3) >>= move >>= move
s = r \\ [(3, 3)]

t = erste 3 3
u = [ t x y| (x, y) <- s]
v (Just (list, n))= n 


dfs :: Table -> [TaggedRoute] -> Limit -> Position -> Maybe (Int, TaggedRoute)
dfs _ [] _ _ = Nothing
dfs table (route:xs) limit goal
    | here == goal || goal `elem` frontier = Just (length table + 1, (goal, 0):route)
    | depth == limit = dfs (here:table) (xs) limit goal
    | otherwise = dfs newTable (map (attachRoute . tag) frontier ++ xs) limit goal
    where   (here, depth):rs = route
            frontier = filter (flip notElem table) . move $ here
            newTable = if here `notElem` table then here:table else table
            tag position = (position, depth + 1)
            attachRoute p = p:route

iddfs n x y x' y'
    | n > 5 = Nothing
    | otherwise = case dfs [] [[((x, y), 0)]] n (x', y') of
            Nothing -> iddfs (succ n) x y x' y'
            Just r -> Just r


--astar route start goal
--    | start == goal = route
--    | otherwise = astar (start:route) (bestNode start) goal
--    where   (x, y) = start
--            (x', y') = goal

tagFitness goal p = (fitness goal p, p)
fitness (x', y') (x, y) = floor $ (abs (fromInteger x' - fromInteger x) + abs (fromInteger y' - fromInteger y)) / 3
--bestNode :: Position -> Position
--bestNode = snd . head . sortBy (comparing fst) . map tagFitness . move

--data Tree a = Node a (Forest a) deriving (Eq, Show)
--type Forest a = [Tree a]

--tree = Node 4 [Node 3 [Node 5 [], Node 6 [], Node 7 []], Node 2 [], Node 1 []]

--dfs stack [] = stack
--dfs stack (Node a branches) = dfs (branches ++ stack)

--dfs (Node a []) = a
--dfs (Node a branches)

--iddfs = 

--data Tree a = Node a (Forest a) | Leaf a deriving (Eq, Show)
--type Forest a = [Tree a]

--type Index = Int
--type Context a = [(Index, a, [Tree a])]
--type Location a = (Tree a, Context a)

--instance Monad Tree where
--    return a = Leaf a
--    Leaf a >>= f = f a
--    Node a branches >>= f = Node b (map (>>= f) branches)
--        where Leaf b = f a 



--a = (Node "haha" [Leaf "ge", Leaf "adf", Leaf "banana"], [])

--type Route = Location Position

--up :: Location a -> Location a
--up (g, []) = (g, [])
--up (g, ((i, father, sibling):xs)) = (Node father sibling', xs)
--    where   sibling' = insert i g sibling
--            insert _ y [] = [y]
--            insert 0 y xs = y:xs
--            insert n y (x:xs) = x : insert (n - 1) y xs 

--down :: Int -> Location a -> Location a
--down _   (Leaf a         , context) = (Leaf a, context)
--down nth (Node a children, context) = (child, family:context)
--    where   nth' = min nth (length children - 1)
--            child = children !! nth'
--            (w, x:y) = splitAt nth' children
--            siblings = w ++ y
--            family = (nth', a, siblings)


--depth :: Location a -> Int
--depth (Leaf a         , context) = 1

--level :: Location a -> Int
--level (_, context) = length context

--order :: Location a -> Int
--order (_, []) = 0
--order (_, (i, _, _):_) = i

----next :: Location a -> Location a
----next (graph, []) = 
----    where   (birthOrder, father, )
----            siblingSize = length context

--hasBigBrother :: Location a -> Bool
--hasBigBrother (_, []) = False
--hasBigBrother (_, (i, a, siblings) : context) = i < length siblings

----hasBigCousin :: Location a -> Bool
----hasBigCousin (_, []) = False
----hasBigCousin location = 
----    where   father = up location
----            hasBigUncle = hasBigBrother father


--findfork (_, []) = 0
--findfork self
--    | hasBigBrother self = 1
--    | otherwise          = 1 + findfork father
--    where   father = up self

--findYoungestChild :: Int -> Location a -> Location a
--findYoungestChild level here
--    | depth here < level

--instance Monad GZipper where
--    return p = GZipper p [] []
--    GZipper p 

--data Route = Route Position [Route] | Focus Position [Route] | Frontier Position deriving (Eq, Show)

move (x, y) = filter onBoard [
        (x + 1, y + 2),
        (x + 2, y + 1),
        (x + 2, y - 1),
        (x + 1, y - 2),
        (x - 1, y - 2),
        (x - 2, y - 1),
        (x - 2, y + 1),
        (x - 1, y + 2)
    ]
    where onBoard (x, y) = x >= 0 && x < 8 && y >= 0 && y < 8


origin = (0, 0)


--origin = Frontier (0, 0)

--expand :: Position -> [Route]
--expand = map Frontier . move

--exploreFrontier :: [Route] -> [Route]
--exploreFrontier [] = []
--exploreFrontier (Frontier f : fs) = Focus f f' : fs
--    where f' = expand f
--exploreFrontier (Focus f f' : fs) = Route f f' : exploreFrontier fs
--exploreFrontier (Route r r' : fs) = Route r r' : exploreFrontier fs

--explore (Frontier s) = Focus s (expand s)
--explore (Focus s f) = Route s (exploreFrontier f)
--explore (Route s )


