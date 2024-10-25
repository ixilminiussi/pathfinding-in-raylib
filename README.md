# Pathfinding
A pathfinding experiment that ended up growing beyond its original scope. This is my first attempt at pathfinding, and optimizing it has been both challenging and rewarding. Below is a breakdown of the project’s aesthetics and optimizations.

<div align="center">
  <!-- Centered YouTube Thumbnail with Link -->
  <a href="https://youtu.be/BZJcOaKEsyQ">
    <img src="https://img.youtube.com/vi/BZJcOaKEsyQ/0.jpg" alt="YouTube Video" style="width:80%; max-width:500px;">
  </a>
  <br>
  <p>

### [Watch the demo](https://youtu.be/BZJcOaKEsyQ)</p>
</div>


## Aesthetics
- **Customizable World Grid:** Users can freely draw the world using a grid. Wall shapes adapt to neighboring walls to create a smooth, less pixelated look.
- **Intuitive UI:** The UI displays key bindings and lets users toggle debugging tools on or off.
- **Special Effects:** Right-clicking triggers a simple effect. The effect API is worth exploring for its elegance and ease of use.
- **Hexagonal movement grid:** Pathfinding operates on a hexagonal grid instead of a square grid, leading to more organic paths.
- **Boid-like movement:** Soldiers' movements are influenced by their assigned paths but also their neighbours. They make sure to align but avoid their neighbours. When they stray too far off and get stuck, they simply regenerate a path.
- **Fisheye Objective Assignment:** When selecting multiple soldiers, their actual destination is unique to each and follows on a fisheye distribution to ensure even spacing and an aesthetically pleasing result.

## Optimisation
- **Hexagonal Grid for Consistent Distance:** Using a hexagonal grid means nodes have 6 neighbours with uniform distance, making it more optimal than a square grid for A*.
- **Multithreaded Pathfinding with Thread Pooling:** Pathfinding is multithreaded using `ctpl` for thread pooling. While a soldier is searching, it skips its update function, letting other soldiers continue theirs. Each A* search creates copies of the nodes they explore to prevent deadlock.
- **Boundary Boxes for Efficient Neighbour Checking:** Moving soldiers, much like boids, need to check for nearby units to avoid impact and align with them. Boundary boxes help by limiting neighbour checks to nearby soldiers only.
- **Memory Safety:** The program makes *heavy* use of shared and unique pointers to ensure robust, memory-safe operations. Essential when working with multithreading, static shared lists and more. This was a great opportunity to deepen my understanding of smart pointers, and tools for debugging memory leaks.
- **Optimized Line Validity Check with Bresenham's algorithm:** A fast line-validation algorithm (using Bresenham's algorithm) only iterates through tiles (Wall or Empty) along a given segment, making it efficient enough to use non-sparingly:
    1. Before using A* we check if a straight line would suffice.
    2. While using A*, check for a straight line every chance we get. If we find a hit, we add this distance to the F weight and keep trying only paths which could outperform our current winner.
    3. Once a path is found, we check for potential straight lines to skip through nodes leading up to our goal, this cuts down the total amount of segments and shortens the distance one last time.



