# Solution-to-15-puzzle-problem-using-A-star-algorithm
AI A* algorithm has been used to solve the 15 puzzle problem:
Heuristics used are  (1) number of misplaced tiles, (2) Sum of “number of moves each tile is away from its goal position”. 
f(n) = g(n) + h(n).
h(n) can be given in two different ways (mentioned above)
g(n) is the distance from the starting state  
final_heuristic1.c is the file with f(n) = g(n) + h(n); where g(n)=num of moves till now and h(n)=num of misplaced tiles
final_heuristic2.c is the file with f(n) = g(n) + h(n); where g(n)=num of moves till now and h(n)=num of moves till goal state is reached
These files take in random generation for the start state board. To check, manual input in the code is commented and can be verified. If manual input is given: shuffle_state(goal_state) must be commented.
