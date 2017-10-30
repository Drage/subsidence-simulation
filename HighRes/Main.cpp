
#include "../Common/CellGrid3D.h"
#include "../Common/SelectionSet.h"
#include "../Common/Timer.h"
#include "../Common/Heightmap.h"
#include "../Common/Xml.h"
#include "../Common/Obj.h"
#include <iostream>

const char EARTH = 'e';
const char AIR  = 'a';
const char COAL = 'c';
const char DRILL = 'd';
const char VOID  = 'v';
const char STATIC_VOID = 's';

void Iterate(CellGrid3D &grid, SelectionSet<Vector3> neighbourhood, float killBubble)
{
	for (int z = grid.GetDepth() - 1; z >= 0; z--)
	{
		for (int x = grid.GetWidth() - 1; x >= 0 ; x--)
		{
			for (int y = grid.GetHeight() - 1; y >= 0; y--)
			{
				Vector3 offset, target;
				
				switch (grid(x, y, z))
				{
					case VOID:			
						offset = neighbourhood.RouletteSelect();
						target = Vector3(x, y, z) + offset;
						if (grid(target) == EARTH || grid(target) == AIR)
						{
							if (Random::Float() < killBubble)
								grid(x, y, z) = STATIC_VOID;
							else 
							{
								grid(x, y, z) = grid(target);
								grid(target) = VOID;
							}
						}
						break;
						
					case DRILL:
						grid(x, y, z) = VOID;
						if (grid(x, y, z + 1) == COAL)
							grid(x, y, z + 1) = DRILL;
						else if (grid(x, y, z - 1) == COAL)
							grid(x, y, z - 1) = DRILL;
						else if (grid(x + 1, y, z) == COAL)
							grid(x + 1, y, z) = DRILL;
						else if (grid(x - 1, y, z) == COAL)
							grid(x - 1, y, z) = DRILL;
						break;
						
					case EARTH:
						if (grid(x, y - 1 , z) == AIR || grid(x, y - 1, z) == STATIC_VOID)
						{
							if (grid(x - 1, y - 1, z) == AIR 
							 || grid(x + 1, y - 1, z) == AIR
							 || grid(x, y - 1, z - 1) == AIR 
							 || grid(x, y - 1, z + 1) == AIR
							 || grid(x - 1, y, z) == AIR 
							 || grid(x + 1, y, z) == AIR
							 || grid(x, y, z - 1) == AIR 
							 || grid(x, y, z + 1) == AIR
							 || grid(x - 1, y - 1, z) == STATIC_VOID 
							 || grid(x + 1, y - 1, z) == STATIC_VOID
							 || grid(x, y - 1, z - 1) == STATIC_VOID 
							 || grid(x, y - 1, z + 1) == STATIC_VOID
							 || grid(x - 1, y, z) == STATIC_VOID 
							 || grid(x + 1, y, z) == STATIC_VOID
							 || grid(x, y, z - 1) == STATIC_VOID 
							 || grid(x, y, z + 1) == STATIC_VOID)
							{
								for (int i = y - 1; i < grid.GetHeight(); i++)
									grid(x, i, z) = grid(x, i + 1, z);
							}
						}
						break;
				}
			}
		}
	}
}

void SaveMesh(CellGrid3D &grid, float smoothing, const std::string outputFile)
{
	// Create heightmap from cell grid
	Heightmap hmap;
	hmap.SetSize(grid.GetWidth(), grid.GetDepth());
	for (int x = 0; x < grid.GetWidth(); x++)
	{
		for (int z = 0; z < grid.GetDepth(); z++)
		{
			int y = grid.GetHeight() - 1;
			while (grid(x, y, z) != EARTH)
				y--;
			
			hmap(x, z) = y;
		}
	}
	hmap.Smooth(smoothing);
	
	// Convert heightmap to mesh
	Mesh *mesh = new Mesh("heightmap");
	Vertex a, b, c, d;
	for (int x = 0; x < hmap.GetWidth() - 1; x++)
	{
		for (int z = 0; z < hmap.GetDepth() - 1; z++)
		{
			a.position = Vector3(x, hmap(x, z + 1), z + 1);
			b.position = Vector3(x + 1, hmap(x + 1, z + 1), z + 1);
			c.position = Vector3(x + 1, hmap(x + 1, z), z);
			d.position = Vector3(x, hmap(x, z), z);
			mesh->AddQuad(a, b, c, d);
		}
	}
	mesh->CalculateNormals();
	
	// Save mesh as wavefront OBJ file
	Obj obj;
	obj.AddMesh(mesh);
	obj.Save(outputFile);
}

int main(int argc, char **argv)
{
	using std::string;
	
	// Read config file
	Xml xml;
	xml.Load("Config.xml");
	
	// Setup grid
	CellGrid3D grid;
	Vector3 dimensions = xml.Get<Vector3>("Grid/Dimensions");
	Vector3 resolution = xml.Get<Vector3>("Grid/Resolution");
	grid.SetSize(dimensions * resolution);
	
	grid.SetBoundMode(xml.Get<string>("Grid/BoundMode/Top"), CellGrid3D::TOP);
	grid.SetBoundMode(xml.Get<string>("Grid/BoundMode/Bottom"), CellGrid3D::BOTTOM);
	grid.SetBoundMode(xml.Get<string>("Grid/BoundMode/Left"), CellGrid3D::LEFT);
	grid.SetBoundMode(xml.Get<string>("Grid/BoundMode/Right"), CellGrid3D::RIGHT);
	grid.SetBoundMode(xml.Get<string>("Grid/BoundMode/Front"), CellGrid3D::FRONT);
	grid.SetBoundMode(xml.Get<string>("Grid/BoundMode/Back"), CellGrid3D::BACK);

	grid.Fill(xml.Get<char>("Grid/DefaultValue"));
	Xml::Element *e = xml.root.GetSubElement("Grid");
	for (Xml::ElementListType::iterator i = e->subElements.begin(); i != e->subElements.end(); i++)
	{
		if ((*i)->name == "Region")
		{
			char value = (*i)->Get<char>("Value");
			Vector3 position = (*i)->Get<Vector3>("Position");
			Vector3 size = (*i)->Get<Vector3>("Dimensions");
			grid.Fill(position * resolution, size * resolution, value);
		}
	}
	
	// Setup selection set (cell neighbourhood)
	Vector2 mean = xml.Get<Vector2>("SelectionSet/Mean");
	Vector2 variance = xml.Get<Vector2>("SelectionSet/Variance");
	int radius = xml.Get<int>("SelectionSet/Radius");
	SelectionSet<Vector3> neighbourhood;
	GenerateSelectionSet(neighbourhood, mean, variance, radius);
	Random::SetSeed();
	
	// Load simulation parameters
	int iterations = xml.Get<int>("Iterations");
	float killBubble = xml.Get<float>("KillBubble");
	float heightmapSmoothing = xml.Get<float>("HeightmapSmoothing");
	
	Timer timer;
	timer.Start();
	
	// Run simulation
	std::cout << "Running simulation...\n";
	for (int i = 1; i <= iterations; i++)
	{
		if (i % (iterations / 10) == 0) 
			std::cout << (10 * (i / ((iterations) / 10))) << "%\n";
			
		Iterate(grid, neighbourhood, killBubble);
	}
	
	// Output model
	std::cout << "Generating model...\n";
	SaveMesh(grid, heightmapSmoothing, "HeightMap.obj");
	std::cout << "Saved file 'HeightMap.obj'\n";
	
	timer.Pause();
	std::cout << "Elapsed time: " << timer.ToString() << std::endl;
	
	return 0;
}
