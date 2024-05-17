There can be N kings on a board of variable size between [1,100]. They all have starting points and target positions. There are a random number of obstacles on the board as well. How do we get them all to their goals?
I use A* as a subroutine for the Constraint Based Search algorithm, which creates a tree that detects conflicts in potential paths for each of these kings, whether or not they will collide, and acts to resolve them. 
It then outputs a plan that shows each step that a king took to get to its goal, unless their is no viable plan. 
