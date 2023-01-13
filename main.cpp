#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class World: public olc::PixelGameEngine
{
public:
	World()
	{
		sAppName = "World Demo";
	}

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::BLUE);
		return true;
	}
};

int main()
{
	World world;
	if (world.Construct(640, 480, 2, 2, false))
		world.Start();
	return 0;
}
