
import Data.List (nub, (\\), sortBy, minimumBy)
import Data.Ord (comparing)

type Depth = Int
type Limit = Int
type Position = (Int, Int)
type Table = [Position]
type Route = [Position]
type TaggedRoute = [(Position, Int)]

type Node = (Position, Position, Int, Int)

type PQueue = [Node]

data Type = BFS | IDS | AStar deriving (Show)

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

bfs :: Table -> [Route] -> Position -> Maybe (Int, Route)
bfs _ [] _ = Nothing
bfs table (x:xs) goal
    | r == goal || goal `elem` frontier = Just (length table + 1, goal:x)
    | otherwise = bfs newTable (xs ++ map attachRoute frontier) goal
    where   (r:rs) = x
            frontier = filter (flip notElem table) . move $ r
            newTable = if r `notElem` table then r:table else table
            attachRoute p = p:x

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

iddfs n (x, y) (x', y')
    | n > 5 = Nothing
    | otherwise = case dfs [] [[((x, y), 0)]] n (x', y') of
            Nothing -> iddfs (succ n) (x, y) (x', y')
            Just (i, r) -> Just (i, untag r)
            where untag = map fst

aStar :: PQueue -> PQueue -> Position -> Position -> (Int, Route)
aStar open closed start goal
    | start == goal         = (0, [start])
    | goal `inPQueue` open  = (length closed, constructPath (closed ++ open) start goal)
    | otherwise             = aStar open' closed' start goal
    where   (openRest, picked) = pick open
            open'              = foldl (addNode closed) openRest (expand goal picked)
            closed'            = picked : closed

            position (p, _, _, _) = p

            inPQueue _ [] = False
            inPQueue p (x:xs)
                | position x == p   = True
                | otherwise         = inPQueue p xs

            findNode p (x:xs)
                | position x == p   = x
                | otherwise         = findNode p xs

            constructPath :: PQueue -> Position -> Position -> Route
            constructPath queue start goal
                | start == goal = [start]
                | otherwise = goal : constructPath queue start goal'
                where   (_, goal', _, _) = findNode goal queue

            pick :: PQueue -> (PQueue, Node)
            pick queue = (queue \\ [picked], picked)
                where   picked = minimumBy (comparing cost) queue

            cost (_, _, g, h) = g + h


            expand :: Position -> Node -> PQueue
            expand goal (position, _, g, h) = map (evalCost g) . move $ position
                where evalCost g p = (p, position, succ g, heuristic goal p)
            heuristic (x', y') (x, y) = floor $ (abs (fromIntegral x' - fromIntegral x) + abs (fromIntegral y' - fromIntegral y)) / 3

            addNode :: PQueue -> PQueue -> Node -> PQueue
            addNode _ [] p = [p]
            addNode closed (x:xs) p
                | here `inPQueue` closed = (x:xs)
                | position == here = if g + h > cost p then p:xs else x:xs
                | otherwise = x : addNode closed xs p
                where   (position, _, g, h) = x
                        (here, parent, _, _) = p

            toNode position = (position, position, 0, 0)


derFunktion :: Type -> Int -> Int -> Int -> Int -> Maybe (Int, Route)
derFunktion BFS x y x' y' = bfs [] [[(x, y)]] (x', y')
derFunktion IDS x y x' y' = iddfs 0 (x, y) (x', y')
derFunktion AStar x y x' y' = Just $ aStar [((x, y), (x, y), 0, 0)] [] (x, y) (x', y')