
#include "../Common/Graphics.h"
#include "../Common/CellGrid3D.h"
#include "../Common/SelectionSet.h"
#include "../Common/Timer.h"
#include "../Common/OrbitCamera.h"
#include "../Common/Input.h"
#include "../Common/Heightmap.h"
#include "../Common/Xml.h"

const char EARTH = 'e';
const char AIR  = 'a';
const char COAL = 'c';
const char DRILL = 'd';
const char VOID  = 'v';
const char STATIC_VOID = 's';

int iterations;
float killBubble;
Vector3 dimensions;
Vector3 resolution;
Vector2 colourRange;
float heightmapSmoothing;
CellGrid3D grid;
SelectionSet<Vector3> neighbourhood;
Timer timer;
OrbitCamera camera;

void HeightmapFromCellGrid(Heightmap &hmap)
{
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
	hmap.Smooth(heightmapSmoothing);
}

void Update(double deltaTime)
{
	if (Key.escape)
		exit(0);
	
	static int iterationCount = 0;
	if (iterationCount++ < iterations)
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

	camera.Update(deltaTime);
}

void Render()
{	
	Heightmap hmap;
	HeightmapFromCellGrid(hmap);
	
	PerspectiveMode(&camera);
	RenderHeightmap(hmap, Vector3(1) / resolution, colourRange[0], colourRange[1]);
	
	//OrthographicMode();
	//RenderText(10, GetWindowHeight() - 20, Font.fixed, timer.ToString(), Colour::White());
}

int main(int argc, char **argv)
{
	using std::string;
	
	// Read config file
	Xml xml;
	xml.Load("Config.xml");
	
	// Setup grid
	dimensions = xml.Get<Vector3>("Grid/Dimensions");
	resolution = xml.Get<Vector3>("Grid/Resolution");
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
	GenerateSelectionSet(neighbourhood, mean, variance, radius);
	Random::SetSeed();
	
	// Load simulation parameters
	iterations = xml.Get<int>("Iterations");
	killBubble = xml.Get<float>("KillBubble");
	colourRange = xml.Get<Vector2>("ColourRange") * resolution.y;
	heightmapSmoothing = xml.Get<float>("HeightmapSmoothing");
	
	// Setup camera
	camera.SetView(Vector3(dimensions.x - 130, dimensions.y, dimensions.z / 2), dimensions / 2);
	
	// Create window
	int windowWidth = xml.Get<int>("Window/Width");
	int windowHeight = xml.Get<int>("Window/Height");
	std::string windowCaption = xml.Get<string>("Window/Caption");
	InitWindow(windowWidth, windowHeight, windowCaption, Colour::Black());
	
	// Run app
	int fps = xml.Get<int>("Window/FPS");
	timer.Start();
	RunApp(fps, Update, Render);
	
	return 0;
}
