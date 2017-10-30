
#include <mpi.h>
#include <iostream>
#include <string>
#include "../Common/Graphics.h"
#include "../Common/CellGrid2D.h"
#include "../Common/SelectionSet.h"
#include "../Common/Random.h"
#include "../Common/Timer.h"
#include "../Common/SharedMemory.h"
#include "../Common/Input.h"
#include "../Common/CmdArgs.h"

const char EARTH = 'e';
const char AIR = 'a';
const char COAL = 'c';
const char DRILL = 'd';
const char VOID = 'v';
const char STATIC_VOID = 's';

CellGrid2D result;

void Update(double deltaTime)
{
	if (Key.escape)
	{
		MPI_Finalize();
		exit(0);
	}
}

void Render()
{
	int cellWidth = GetWindowWidth() / result.GetWidth();
	int cellHeight = GetWindowHeight() / result.GetHeight();
	
	for (int x = 0; x < result.GetWidth(); x++)
	{
		for (int y = 0; y < result.GetHeight(); y++)
		{
			Colour colour;
			switch (result(x, y))
			{
				case EARTH:
					colour = Colour::LightBlue();
					break;
				case COAL:
					colour = Colour::Blue();
					break;
				case AIR:
				case VOID:
				case STATIC_VOID:
					colour = Colour::White();
					break;
				case DRILL:
					colour = Colour::Black();
					break;
			}
			RenderRectangle(x * cellWidth, y * cellHeight, cellWidth, cellHeight, colour);
		}
	}
}
				 
int main(int argc, char **argv)
{	
	// Set default parameters
	CmdArgs args(argc, argv);
	args.SetDefault("rx", 1);	// Resolution X
	args.SetDefault("ry", 1);	// Resolution Y
	args.SetDefault("w", 600);	// Width
	args.SetDefault("h", 100);	// Height
	args.SetDefault("i", 500);	// Iterations
	args.SetDefault("csh", 10);	// Coal Seam Height
	args.SetDefault("dl", 240);	// Drill Length
	args.SetDefault("gh", 90);	// Ground Height
	
	// Load parameters
	int xRes = args.Get<int>("rx");
	int yRes = args.Get<int>("ry");
	int width = args.Get<int>("w") * xRes;
	int height = args.Get<int>("h") * yRes;
	int iterations = args.Get<int>("i");
	int coalSeamHeight = args.Get<int>("csh") * yRes;
	int drillLength = args.Get<int>("dl") * xRes;
	int groundHeight = args.Get<int>("gh") * yRes;
	float killBubble = 0.2f / (groundHeight - coalSeamHeight * 2);
	
	// Init MPI
	int processorIndex;
	int numProcessors;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcessors);
	MPI_Comm_rank(MPI_COMM_WORLD, &processorIndex);
	
	if (processorIndex == 0)
		std::cout << "MPI initialised with " << numProcessors << " processors.\n";
	
	// Check if there are enough processors
	if (numProcessors < 2)
	{
		std::cout << "Error. A minimum of 2 processors are required to run.\n";
		MPI_Finalize();
		exit(1);
	}
	
	// Create processor subgroup for sector processors
	MPI_Group all;
	MPI_Group sectorsGroup;
	MPI_Comm sectorsComm;
	int exclude[] = { numProcessors - 1 };
	MPI_Comm_group(MPI_COMM_WORLD, &all);
	MPI_Group_excl(all, 1, exclude, &sectorsGroup);
	MPI_Comm_create(MPI_COMM_WORLD, sectorsGroup, &sectorsComm); 
	
	if (processorIndex == numProcessors - 1)
	{
		RunSharedMemoryServer(numProcessors - 1, width);
	}
	else
	{	
		int numSectors = numProcessors - 1;
		
		// Set sector dimensions
		int sectorWidth = width;
		int sectorHeight = height / numSectors + 1;
		
		// No shared row on bottom
		if (processorIndex == 0)
			sectorHeight--;
		
		// Add height remainder to last processor's sector
		if (processorIndex == numSectors - 1)
			sectorHeight += height % numSectors;
		
		// Set sector position
		int sectorX = 0;
		int sectorY = processorIndex * (height / numSectors - 1);
		if (processorIndex == numSectors - 1)
			sectorY = processorIndex * (height / numSectors) - 1;
		
		// Init cell grid values
		CellGrid2D grid;
		grid.SetSize(sectorWidth, sectorHeight);
		grid.SetBoundMode(CellGrid2D::WRAP, CellGrid2D::LEFT);
		grid.SetBoundMode(CellGrid2D::WRAP, CellGrid2D::RIGHT);
		grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::TOP);
		grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::BOTTOM);
		grid.Fill(EARTH);
		grid.FillRect(0, groundHeight - sectorY, width, height - groundHeight, AIR);
		grid.FillRect(0, -sectorY, width, coalSeamHeight, COAL);
		grid.FillRect((width - drillLength) / 2, -sectorY, 1, coalSeamHeight, DRILL);
		
		// Init shared memory
		SharedMemory shm(numProcessors - 1, width);
		shm.Init(processorIndex, grid.GetRow(sectorHeight - 1));
		
		// Init random neighbour selection
		Random::SetSeed(); 
		SelectionSet<int> neighbourhood;
		GenerateSelectionSet(neighbourhood, 0.0, 3.0, -3, 3);
		
		if (processorIndex == 0)
			std::cout << "Running simulation...\n";
		
		Timer timer;
		timer.Start();
		for (int i = 1; i <= iterations; i++)
		{				
			// Print simulation percentage completed
			if (processorIndex == 0 && i % (iterations / 10) == 0)
				std::cout << (10 * (i / ((iterations) / 10))) << "%\n";
			
			for (int y = grid.GetHeight() - 2; y >= 0; y--)
			{
				// Obtain required shared memory locks
				if (y == grid.GetHeight() - 2)
					shm.Obtain(processorIndex, grid.GetRow(sectorHeight - 1));
				if (y == 0 && processorIndex != 0)
					shm.Obtain(processorIndex - 1, grid.GetRow(0));
				
				// Update row
				for (int x = grid.GetWidth() - 1; x >= 0; x--)
				{
					int offset;
					switch (grid(x, y))
					{
						case VOID:
							offset = neighbourhood.RouletteSelect();
							if (grid(x + offset, y + 1) == EARTH || grid(x + offset, y + 1) == AIR)
							{
								if (Random::Float() < killBubble)
									grid(x, y) = STATIC_VOID;
								else
								{
									grid(x, y) = grid(x + offset, y + 1);
									grid(x + offset, y + 1) = VOID;
								}
							}
							break;
						case DRILL:
							grid(x, y) = VOID;
							if (x < (width - drillLength) / 2 + drillLength)
								grid(x + 1, y) = DRILL;
							break;
						case EARTH:
							if (grid(x, y - 1) == AIR || grid(x, y - 1) == STATIC_VOID)
							{
								if (grid(x - 1, y - 1) == AIR || grid(x + 1, y - 1) == AIR
									|| grid(x - 1, y) == AIR || grid(x + 1, y) == AIR
									|| grid(x - 1, y - 1) == STATIC_VOID || grid(x + 1, y - 1) == STATIC_VOID
									|| grid(x - 1, y) == STATIC_VOID || grid(x + 1, y) == STATIC_VOID)
								{
									for (int i = y - 1; i < grid.GetHeight() - 1; i++)
										grid(x, i) = grid(x, i + 1);
								}
							}
							break;
					}
				}
				
				// Release shared memory locks
				if (y == grid.GetHeight() - 2 || (y == 0 && processorIndex != 0))
					shm.Release();
			}
			
			// Sync before next iteration
			MPI_Barrier(sectorsComm);
		}
		
		if (processorIndex != 0)
		{
			// Send final data to master-processor
			int size = grid.GetWidth() * grid.GetHeight();
			MPI_Send(grid.GetRawData(), size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
		}
		else
		{
			std::cout << "Collecting results...\n";
			
			// Create cellgrid to hold final data
			result.SetSize(width, height);
			result.SetBoundMode(CellGrid2D::EXCEPTION);
			result.CopyCells(grid.GetRawData(), width, height / numSectors, 0, 0);
			
			// Recieve final data from all other sector processors
			int bufferSize = sectorWidth * sectorHeight * 2;
			char *cellData = new char[bufferSize];
			for (int i = 1; i < numSectors; i++)
			{	
				MPI_Recv(cellData, bufferSize, MPI_CHAR, i, 0, MPI_COMM_WORLD, 0);
				
				int copyHeight = height / numSectors + 1;
				if (i == numSectors - 1) copyHeight += height % numProcessors;
				
				int copyY = i * (height / numSectors - 1);
				if (processorIndex == numSectors - 1)
					copyY = i * (height / numSectors) - 1;
				
				result.CopyCells(cellData, width, copyHeight, 0, copyY);
			}
			delete[] cellData;
			
			timer.Pause();
			std::cout << "Elapsed time: " << timer.ToString() << std::endl;
			
			shm.Terminate();
			
			// Display result in OpenGL window
			InitWindow(width, height, "Cellular Automata Test", Colour::White());
			RunApp(30, Update, Render);
		}
	}
	
	MPI_Finalize();
	return 0;
}
