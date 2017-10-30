
#include <iostream>
#include <map>
#include "../Common/Graphics.h"
#include "../Common/CellGrid2D.h"
#include "../Common/SelectionSet.h"
#include "../Common/Random.h"
#include "../Common/Input.h"
#include "../Common/Png.h"
#include "../Common/Xml.h"

using std::string;
using std::map;

const char VOID = 0;
const char STATIC_VOID = 1;
const char COAL = 2;
const char DRILL = 3;
const char AIR = 4;
const char CUSTOM = 5;

const int SS_SIZE = 5;

struct CellType
{
	Colour colour;
	float killBubble;
	SelectionSet<int> selectionSet;
	
	CellType() { killBubble = 0; }
	CellType(const Colour &c) 
	{ 
		colour = c; 
		killBubble = 0; 
		GenerateSelectionSet(selectionSet, 0.0, 3.0, -SS_SIZE, SS_SIZE);
	}
};
typedef struct CellType CellType;

typedef map<char, CellType> CellTypeMap;
CellTypeMap cellType;

CellGrid2D grid;

void UpdateDrill(int x, int y)
{
	// Spawn void cell
	grid(x, y) = VOID;
	
	// Move drill onto adjacent coal cell
	// Order of movement is down, right, up, left
	if (grid(x, y - 1) == COAL)
		grid(x, y - 1) = DRILL;
	else if (grid(x + 1, y) == COAL)
		grid(x + 1, y) = DRILL;
	else if (grid(x, y + 1) == COAL)
		grid(x, y + 1) = DRILL;
	else if (grid(x - 1, y) == COAL)
		grid(x - 1, y) = DRILL;
}

void UpdateVoid(int x, int y)
{
	// Use probability distribution for cell type about to move onto
	char type;
	int offset = 0;
	do
	{
		type = grid(x + offset, y + 1);
		offset = cellType[type].selectionSet.RouletteSelect();
	} 
	while (grid(x + offset, y + 1) != type);
	
	if (type == AIR || type >= CUSTOM)
	{
		// chance of converting to static void
		if (Random::Float() < cellType[type].killBubble)
			grid(x, y) = STATIC_VOID;
		else 
		{
			// Swap value with neighbour chosen using selection set
			grid(x, y) = grid(x + offset, y + 1);
			grid(x + offset, y + 1) = VOID;
		}
	}
}

void UpdateCustom(int x, int y)
{
	// Compress down if air or static void is underneath - prevent floating blocks
	if (grid(x, y - 1) == AIR || grid(x, y - 1) == STATIC_VOID)
	{
		// At least one of the cells to the left, right, lower-left or lower-right
		// must also be air/static void for compression to occur
		if (grid(x - 1, y - 1) == AIR || grid(x + 1, y - 1) == AIR
			|| grid(x - 1, y) == AIR || grid(x + 1, y) == AIR
			|| grid(x - 1, y - 1) == STATIC_VOID || grid(x + 1, y - 1) == STATIC_VOID
			|| grid(x - 1, y) == STATIC_VOID || grid(x + 1, y) == STATIC_VOID)
		{
			// Compress all cells above downward
			for (int i = y - 1; i < grid.GetHeight(); i++)
				grid(x, i) = grid(x, i + 1);
		}
	}
}

void Update(double deltaTime)
{
	if (Key.escape)
		exit(0);
	
	for (int x = grid.GetWidth() - 1; x >= 0 ; x--)
	{
		for (int y = grid.GetHeight() - 1; y >= 0; y--)
		{
			switch (grid(x, y))
			{
				case DRILL:	
					UpdateDrill(x, y);	
					break;
				case VOID:	
					UpdateVoid(x, y);	
					break;
				case AIR:
				case STATIC_VOID:
				case COAL:
					break;
				default:	
					UpdateCustom(x, y);	
					break;
			}
		}
	}
}

void Render()
{
	for (int x = 0; x < grid.GetWidth(); x++)
	{
		for (int y = 0; y < grid.GetHeight(); y++)
		{
			RenderRectangle(x, y, 1, 1, cellType[grid(x, y)].colour);
		}
	}
}

int main(int argc, char **argv)
{
	// Load config file
	string configFile = (argc == 2) ? argv[1] : "Config.xml";
	Xml xml(configFile);
	
	// Setup default cell types
	map<Colour, char> cvMap;
	CellType cct(xml.Get<Colour>("Coal"));
	cellType[COAL] = cct;
	cvMap[cct.colour] = COAL;
	CellType dct(xml.Get<Colour>("Drill"));
	cellType[DRILL] = dct;
	cvMap[dct.colour] = DRILL;
	CellType act(xml.Get<Colour>("Air"));
	cellType[AIR] = act;
	cvMap[act.colour] = AIR;
	CellType vct(Colour::White());
	cellType[VOID] = vct;
	CellType svct(Colour::White());
	cellType[STATIC_VOID] = svct;
	
	// Load custom cell types and generate colour-value map
	char value = CUSTOM;
	Xml::ElementListType elements = xml.root.subElements;
	for (Xml::ElementListType::iterator i = elements.begin(); i != elements.end(); i++)
	{
		if ((*i)->name == "CellType")
		{
			CellType ct;
			ct.colour = (*i)->Get<Colour>("Colour");
			ct.killBubble = (*i)->Get<float>("KillBubble");
			float mean = (*i)->Get<float>("ProbabiltyDistribution/Mean");
			float variance = (*i)->Get<float>("ProbabiltyDistribution/Variance");
			GenerateSelectionSet(ct.selectionSet, mean, variance, -SS_SIZE, SS_SIZE);
			cellType[value] = ct;
			
			cvMap[ct.colour] = value;
			value++;
		}
	}
	
	// Init cell grid
	Png png(xml.Get<string>("CellMap"));
	int width = png.GetWidth();
	int height = png.GetHeight();
	grid.SetSize(width, height);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
			grid(x, y) = cvMap[png.GetPixel(x, y)];
	}
	
	// Init random selection
	Random::SetSeed();
	
	// Cell grid bound modes
	grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::TOP);
	grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::BOTTOM);
	grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::LEFT);
	grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::RIGHT);
	
	// Setup OpenGL window
	InitWindow(width, height, "Cellular Automata Simulation", Colour::White());
	RunApp(60, Update, Render);
	return 0;
}
