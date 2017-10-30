# subsidence-simulation
Simulates subsidence caused by longwall mining using a cellular automata model intended to by run on a supercomputer

**MPI must be installed to run these programs** - 
[Installing MPI on OSX](https://wiki.helsinki.fi/display/HUGG/Open+MPI+install+on+Mac+OS+X)

## Introduction

Subsidence, the motion of a surface as it shifts downward, is often caused by mining operations that extract layers of minerals from underneath the earth’s surface. The displacement of the ground surface caused when a void created from mining collapses is both an environmental and infrastructure concern. A Cellular Automaton (CA) model for subsidence prediction iteratively simulates the collapse of a void using a grid of cells, each of which is a Finite State Machine that transitions between states based on the states of neighbouring cells. Accurate models utilize a large number of cells in a three-dimensional array - a computationally intensive simulation that is impractical to run on a normal machine. To mitigate the large computational times associated with such a simulation these programs explore the possibility of parallel processing using MPI and determine the extent that supercomputing facilities could improve the execution time of the model.

## Animated2D
*Animated 2D test program for visual analysis of CA behavior*

Renders a Cellular Automata simulation in real time – one iteration of the cell grid per frame. Each cell type is represented by a different colored square: dark blue (coal), light blue (earth), black (drill), and white (air/void). The elapsed time is displayed in the top-right corner, although this is not an accurate representation of the required computational time (rendering slows execution substantially).

![anim2d]()

### Usage
```
./anim2d –w 600 –h 110 –rx 2 –ry 2 –csh 5 –dl 240 –gh 100
```

Parameter | Default Value | Description
--- | --- | ---
w | 600 | Width of the cell grid (and application window)
h | 300 | Height of the cell grid (and application window)
rx | 1 | Resolution of the cell grid in the x axis (cells per unit size)
ry | 1 | Resolution of the cell grid in the y axis (cells per unit size)
csh | 50 | Height (thickness) of the coal seam (must be smaller than ground height)
dl | 400 | Drill length – how far the drill moves (must be smaller than width)
gh | 200 | Ground height – where the earth stops and the air starts (must be smaller than height)


