# subsidence-simulation
Simulates subsidence caused by longwall mining using a cellular automata model intended to by run on a supercomputer

**MPI must be installed to run these programs** - 
[Installing MPI on OSX](https://wiki.helsinki.fi/display/HUGG/Open+MPI+install+on+Mac+OS+X)


## Introduction

Subsidence, the motion of a surface as it shifts downward, is often caused by mining operations that extract layers of minerals from underneath the earth’s surface. The displacement of the ground surface caused when a void created from mining collapses is both an environmental and infrastructure concern. A Cellular Automaton (CA) model for subsidence prediction iteratively simulates the collapse of a void using a grid of cells, each of which is a Finite State Machine that transitions between states based on the states of neighbouring cells. Accurate models utilize a large number of cells in a three-dimensional array - a computationally intensive simulation that is impractical to run on a normal machine. To mitigate the large computational times associated with such a simulation these programs explore the possibility of parallel processing using MPI and determine the extent that supercomputing facilities could improve the execution time of the model.


## Animated2D
*Animated 2D test program for visual analysis of CA behavior*

Renders a Cellular Automata simulation in real time – one iteration of the cell grid per frame. Each cell type is represented by a different colored square: dark blue (coal), light blue (earth), black (drill), and white (air/void). The elapsed time is displayed in the top-right corner, although this is not an accurate representation of the required computational time (rendering slows execution substantially).

![2D Output](https://github.com/Drage/subsidence-simulation/blob/master/img/anim2d.png)

### Usage
```
./anim2d –w 600 –h 110 –rx 2 –ry 2 –csh 5 –dl 240 –gh 100
```

### Parameters

Parameter | Default Value | Description
--- | --- | ---
w | 600 | Width of the cell grid (and application window)
h | 300 | Height of the cell grid (and application window)
rx | 1 | Resolution of the cell grid in the x axis (cells per unit size)
ry | 1 | Resolution of the cell grid in the y axis (cells per unit size)
csh | 50 | Height (thickness) of the coal seam (must be smaller than ground height)
dl | 400 | Drill length – how far the drill moves (must be smaller than width)
gh | 200 | Ground height – where the earth stops and the air starts (must be smaller than height)

![Dimensions](https://github.com/Drage/subsidence-simulation/blob/master/img/dimensions.png)


## Animated3D
*Animated 3D test program for visual analysis of CA behavior*

Renders a Cellular Automata simulation in real time – one iteration of the cell grid per frame. The Cell Grid is represented by a height-map, where the highest ‘Earth’ cell determines the y-axis location of each point. Vertex coloring also indicates height with lower vertices colored red, fading to green for higher points.

![3D Output](https://github.com/Drage/subsidence-simulation/blob/master/img/anim3d.png)

### Usage
```
./anim3d
```

Control | Action
--- | ---
Escape | Close
Left Mouse Button/Arrow Keys | Rotate Camera
Right Mouse Button | Zoom

### Parameters
Parameters are specified as XML markup in Config.xml. All tags should exist inside of <CellularAutomata></CellularAutomata> root tags. In regards to types, parameters expecting a character accept strings, although only the first character is used (allowing cell types to be named, but represented by a single byte e.g. ‘earth’ is represented by the ASCII value for ‘e’). Parameters expecting a Vector2 or Vector3 require two or three decimal numbers respectively, separated by spaces.

Path | Type | Description
--- | --- | ---
Window | - | Defines application window parameters
Window/Width | Integer | Width of window
Window/Height | Integer | Height of window
Window/Caption | String | Text displayed in window toolbar
Window/FPS | Integer | Frame rate of simulation
Grid | - | Defines cell grid parameters
Grid/Dimensions | Vector3 | Size of the CA (in meters)
Grid/Resolution | Vector3 | Resolution of the CA (cells per unit size)
Grid/BoundMode/Border | String | The behavior of cells outside the boundary of the grid. May be set to ignore, wrap, exception, or border #. (In the case of border, the # should be replaced by whatever character will be the border cell value.)
Grid/DefaultValue | Character | The default cell value for all cells in the grid
Grid/Region | - | Defines a rectangular section of cells
Grid/Region/Value | Character | Value of all cells in the region
Grid/Region/Position | Vector3 | The location of the region in the grid (bottom-left coordinate)
Grid/Region/Dimensions | Vector3 | Size of the region
SelectionSet | - | Defines roulette-wheel selection parameters for horizontal void cell movement.
SelectionSet/Mean | Vector2 | The mean of the Gaussian distribution used
SelectionSet/Variance | Vector2 | The variance of the Gaussian distribution used
SelectionSet/Radius | Integer | The size of the cell neighborhood
Iterations | Integer | The number of iterations to run the simulation before stopping
KillBubble | Float | The probability of a void cell (bubble) becoming a static void cell each time it moves upwards
ColourRange | Vector2 | The min and max heights for the heightmap colors to range over – min is red, max is green
HeightmapSmoothing | Float | The amount of smoothing to do on the heightmap (0 = no smoothing, 1 = max smoothing)


## VisualMPI
*Displays results of MPI test run visually (2D)*

This program renders the output of a CA simulation after its completion. Rendering is the same as in Animated2D. Computation of the CA uses MPI, although it is intended for Mac/Unix (not a supercomputer). This assumes that an MPI library is installed on the local machine. Distribution of computation to multiple processors is achieved using horizontal subdivision of the cell grid. The program must be run with at least two processors as one is used as a shared memory server. The final cell data contained on each processor is collated after the required number of iterations is completed – all processors send the data to the processor with an index of 0, which then displays the complete data set. This program is used to visually check that an MPI algorithm is working as intended.

### Usage
```
mpirun –np 4 ./vismpi –i 1000 –rx 2 –ry 2
```


## HighRes
*Generates a model file of a 3D CA simulation*

This program outputs a high-resolution model of the ground topology that results from a 3D CA simulation. The model is in the format of a Wavefront OBJ file, allowing it to be opened and viewed using virtually any 3D modeling software.
The program does not use MPI; it uses only a single processing thread.

### Usage
```
./highres
```


## Composite
*A variation of Animated2D that allows for different material types.*

Renders a Cellular Automata simulation in real time – one iteration of the cell grid per frame. Loads the initial configuration from an xml file that specifies cell types properties and an image file that defines initial cell states. Each cell type may have a different mean and variance for the probability distribution and kill-bubble probability. The image supplied is converted to cell grid values based on the pixel color.

### Usage
```
./ca ConfigFile.xml
```

### Example config XML file
```
<CellularAutomata>
  <CellMap>CellMap2.png</CellMap>
  
  <Coal>0x03417A</Coal>
  <Drill>0x000000</Drill>
  <Air>0xFFFFFF</Air>
  
  <CellType>
    <Colour>0x4697DF</Colour>
    <ProbabiltyDistribution>
      <Mean>0.0</Mean>
      <Variance>3.0</Variance>
     </ProbabiltyDistribution>
     <KillBubble>0.002</KillBubble>
  </CellType>
  
  <CellType>
    <Colour>0x0A5394</Colour>
    <ProbabiltyDistribution>
      <Mean>0.0</Mean>
      <Variance>0.0001</Variance>
     </ProbabiltyDistribution>
    <KillBubble>0.0</KillBubble>
  </CellType>
  
  <CellType>
    <Colour>0x46b9df</Colour>
    <ProbabiltyDistribution>
      <Mean>-2.0</Mean>
      <Variance>3.0</Variance>
    </ProbabiltyDistribution>
    <KillBubble>0.0</KillBubble>
  </CellType>
</CellularAutomata>
```
