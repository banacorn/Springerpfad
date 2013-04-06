

type Position = (Int, Int)
type Table = (Int, [Position])

type Route = [Position]

bfs :: Table -> [Route] -> Position -> Maybe (Int, Route)
bfs _ [] _ = Nothing
bfs (i, table) (x:xs) target
    | r == target = Just (i, x)
    | otherwise = bfs (succ i, r:table) (xs ++ frontier) target
    where   (r:rs) = x
            frontier = map attachRoute . filter (flip notElem table) . move $ r
            attachRoute p = p:x

a = bfs (0, []) [[(0, 0)]] (2, 3)
b = bfs (0, []) [[(0, 0)]] (2, 2)

data Tree a = Node a [Tree a] deriving (Eq, Show)


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
        (x + 1, y - 2),
        (x - 1, y + 2),
        (x - 1, y - 2),
        (x + 2, y + 1),
        (x + 2, y - 1),
        (x - 2, y + 1),
        (x - 2, y - 1)
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


