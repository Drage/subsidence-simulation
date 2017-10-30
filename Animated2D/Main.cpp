
#include <stdio.h>
#include "../Common/Graphics.h"
#include "../Common/CellGrid2D.h"
#include "../Common/SelectionSet.h"
#include "../Common/Random.h"
#include "../Common/Timer.h"
#include "../Common/Input.h"
#include "../Common/CmdArgs.h"

const char EARTH = 'e';
const char AIR = 'a';
const char COAL = 'c';
const char DRILL = 'd';
const char VOID = 'v';
const char STATIC_VOID = 's';

CellGrid2D grid;
SelectionSet<int> neighbourhood;
Timer timer;
int width;
int height;
float drawScale;
int drillLength;
float killBubble;

void Update(double deltaTime)
{
	if (Key.escape)
		exit(0);
	
	// Update from top-right to bottom-left
	// This prevents void and drill cells from being updated multiple times in a single iteration
	for (int x = grid.GetWidth() - 1; x >= 0 ; x--)
	{
		for (int y = grid.GetHeight() - 1; y >= 0; y--)
		{
			int offset;
			switch (grid(x, y))
			{
				case VOID:
					offset = neighbourhood.RouletteSelect();
					// move 'bubble' up through earth and air - do not move through coal or static voids
					if (grid(x + offset, y + 1) == EARTH || grid(x + offset, y + 1) == AIR)
					{
						// small chance of getting stuck - convert to static void
						if (Random::Float() < killBubble)
							grid(x, y) = STATIC_VOID;
						else 
						{
							// Swap value with neighbour chosen using selection set
							grid(x, y) = grid(x + offset, y + 1);
							grid(x + offset, y + 1) = VOID;
						}
					}
					break;
				case DRILL:
					// Create void cell where coal was, move right one
					grid(x, y) = VOID;
					if (x < (width - drillLength) / 2 + drillLength)
						grid(x + 1, y) = DRILL;
					break;
				case EARTH:
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
							for (int i = y - 1; i < height; i++)
								grid(x, i) = grid(x, i + 1);
							//char temp = grid(x, y - 1);
							//grid(x, y - 1) = grid(x, y);
							//grid(x, y) = temp;
						}
					}
					break;
			}
		}
	}
}

void Render()
{
	RenderRectangle(50, 50, width, height, Colour::White());
	
	int cellWidth = width / grid.GetWidth() * drawScale;
	int cellHeight = height / grid.GetHeight() * drawScale;
	
	for (int x = 0; x < grid.GetWidth(); x++)
	{
		for (int y = 0; y < grid.GetHeight(); y++)
		{
			char value = grid(x, y);
			Colour colour;
			
			switch (value)
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
				default:
					colour = Colour::White();
					break;
			}

			RenderRectangle(x * cellWidth + 50, y * cellHeight + 50, cellWidth, cellHeight, colour);
		}
	}
	
	//RenderText(10, height * drawScale - 70, Font.fixed, timer.ToString(), Colour::Black());
}

int main(int argc, char **argv)
{
	// Set defualt parameters
	CmdArgs args(argc, argv);
	args.SetDefault("rx", 1);	// Resolution X
	args.SetDefault("ry", 1);	// Resolution Y
	args.SetDefault("w", 600);	// Width
	args.SetDefault("h", 300);	// Height
	args.SetDefault("csh", 50);	// Coal Seam Height
	args.SetDefault("dl", 400);	// Drill Length
	args.SetDefault("gh", 200);	// Ground Height
	args.SetDefault("ds", 2);	// Draw Scale
	
	// Load parameters
	int xRes = args.Get<int>("rx");
	int yRes = args.Get<int>("ry");
	width = args.Get<int>("w") * xRes;
	height = args.Get<int>("h") * yRes;
	drawScale = args.Get<float>("ds");
	int coalSeamHeight = args.Get<int>("csh") * yRes;
	drillLength = args.Get<int>("dl") * xRes;
	int groundHeight = args.Get<int>("gh") * yRes;
	killBubble = 0.2f / (groundHeight - coalSeamHeight);
	
	// Init selection set
	Random::SetSeed(); 
	GenerateSelectionSet(neighbourhood, 0.0, 3.0, -3, 3);
	
	// Init cell grid
	grid.SetSize(width, height);
	grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::TOP);
	grid.SetBoundMode(CellGrid2D::IGNORE, CellGrid2D::BOTTOM);
	grid.Fill(EARTH);
	grid.FillRect(0, groundHeight, width, height - groundHeight, AIR);
	grid.FillRect(0, 0, width, coalSeamHeight, COAL);
	grid.FillRect((width - drillLength) / 2, 0, 1, coalSeamHeight, DRILL);
	
	// Setup OpenGL window
	InitWindow(drawScale * width + 100, drawScale * height + 100, "Cellular Automata Test", Colour::Black());
	timer.Start();
	RunApp(60, Update, Render);
	return 0;
}
