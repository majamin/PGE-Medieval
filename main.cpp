#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

// Override base class with your custom functionality
class IsometricDemo : public olc::PixelGameEngine
{
  public:
    IsometricDemo()
    {
      sAppName = "Coding Quickie: Isometric Tiles";
    }

  private:
    // Number of tiles in world
    olc::vi2d vWorldSize = { 14, 10 };

    // Size of single tile graphic
    olc::vi2d vTileSize = { 64, 32 };

    // Where to place tile (0,0) on screen (in tile size steps)
    olc::vi2d vOrigin = { 4, 1 };

    // Sprite that holds all imagery
    olc::Sprite *sprIsom = nullptr;
    olc::Sprite *sprIsomSelected = nullptr;
    olc::Sprite *colorCheat = nullptr;

    // Pointer to create 2D world array
    int *pWorld = nullptr;

    // NOTE: used in calculation to select tiles
    int tilesMax = 80;
    int tilesPerRow;

    //
    int paintSelection = 0;

  public:
    bool OnUserCreate() override
    {
      // Load sprites used in demonstration
      sprIsom = new olc::Sprite("./assets/iso-64x64-building_2.png");
      sprIsomSelected = new olc::Sprite("./assets/iso-64x64-building-standout.png");
      colorCheat = new olc::Sprite("./assets/color_cheat.png");

      // NOTE: An array can't be entirely initialized with a non-zero value.
      //       We need a for loop (c++ newbie things)!
      pWorld = new int[vWorldSize.x * vWorldSize.y]{ 0 };
      for(int i = 0; i < vWorldSize.x * vWorldSize.y; i++) {
        pWorld[i] = 2;
      }

      // calculate how many tiles per row
      tilesPerRow = (sprIsom->width / vTileSize.x);

      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
      Clear(olc::WHITE);

      // Labmda function to convert "world" coordinate into screen space
      auto ToScreen = [&](int x, int y)
      {
        return olc::vi2d
        {
          (vOrigin.x * vTileSize.x) + (x - y) * (vTileSize.x / 2),
          (vOrigin.y * vTileSize.y - vTileSize.y) + (x + y) * (vTileSize.y / 2)
        };
      };

      // Get Mouse in world
      olc::vi2d vMouse = { GetMouseX(), GetMouseY() };

      // Work out active cell
      olc::vi2d vCell = { vMouse.x / vTileSize.x, vMouse.y / vTileSize.y };

      // Work out mouse offset into cell
      olc::vi2d vOffset = { vMouse.x % vTileSize.x, vMouse.y % vTileSize.y };

      // Sample into cell offset colour
      olc::Pixel col = colorCheat->GetPixel(0 * vTileSize.x + vOffset.x, vOffset.y);

      // Work out selected cell by transforming screen cell
      olc::vi2d vSelected =
      {
        (vCell.y - vOrigin.y) + (vCell.x - vOrigin.x),
        (vCell.y - vOrigin.y) - (vCell.x - vOrigin.x)
      };

      // "Bodge" selected cell by sampling corners
      if (col == olc::RED) vSelected += {-1, +0};
      if (col == olc::BLUE) vSelected += {+0, -1};
      if (col == olc::GREEN) vSelected += {+0, +1};
      if (col == olc::YELLOW) vSelected += {+1, +0};

      // Handle mouse click to toggle if a tile is visible or not
      if (GetMouse(0).bPressed)
      {
        // Guard array boundary
        if (vSelected.x >= 0 && vSelected.x < vWorldSize.x && vSelected.y >= 0 && vSelected.y < vWorldSize.y)
          pWorld[vSelected.y * vWorldSize.x + vSelected.x] = paintSelection;
      }

      if (GetMouseWheel() > 0) paintSelection = (++paintSelection) % tilesMax;
      if (GetMouseWheel() < 0) paintSelection = std::max(0, --paintSelection) % tilesMax;

      if (GetKey(olc::W).bPressed) vOrigin.y += 1;
      if (GetKey(olc::S).bPressed) vOrigin.y -= 1;
      if (GetKey(olc::A).bPressed) vOrigin.x += 1;
      if (GetKey(olc::D).bPressed) vOrigin.x -= 1;

      // Draw World - has binary transparancy so enable masking
      SetPixelMode(olc::Pixel::MASK);

      // Convert selected cell coordinate to world space
      olc::vi2d vSelectedWorld = ToScreen(vSelected.x, vSelected.y);

      // (0,0) is at top, defined by vOrigin, so draw from top to bottom
      // to ensure tiles closest to camera are drawn last
      for (int y = 0; y < vWorldSize.y; y++)
      {
        for (int x = 0; x < vWorldSize.x; x++)
        {
          // Convert cell coordinate to world space
          olc::vi2d vWorld = ToScreen(x, y);
          int index = pWorld[y*vWorldSize.x + x];

          if (x == vSelected.x && y == vSelected.y)
            // Draw cursor
            DrawPartialSprite(vSelectedWorld.x, vSelectedWorld.y, sprIsomSelected, paintSelection * vTileSize.x % sprIsomSelected->width, ((paintSelection / tilesPerRow ) * vTileSize.y * 2) % sprIsomSelected->height, vTileSize.x, vTileSize.y * 2);
          else
            // Normal tile
            DrawPartialSprite(vWorld.x, vWorld.y, sprIsom, index * vTileSize.x % sprIsom->width, ((index / tilesPerRow ) * vTileSize.y * 2) % sprIsom->height, vTileSize.x, vTileSize.y * 2);
        }
      }

      // Draw Selected Cell - Has varying alpha components
      SetPixelMode(olc::Pixel::ALPHA);

      // Go back to normal drawing with no expected transparency
      SetPixelMode(olc::Pixel::NORMAL);

      // Draw Hovered Cell Boundary
      //DrawRect(vCell.x * vTileSize.x, vCell.y * vTileSize.y, vTileSize.x, vTileSize.y, olc::RED);

      // Draw Debug Info
      DrawString(4, 4, "Mouse   : " + std::to_string(vMouse.x) + ", " + std::to_string(vMouse.y), olc::BLACK);
      DrawString(4, 14, "Cell    : " + std::to_string(vCell.x) + ", " + std::to_string(vCell.y), olc::BLACK);
      DrawString(4, 24, "Selected: " + std::to_string(vSelected.x) + ", " + std::to_string(vSelected.y), olc::BLACK);
      // DrawString(4, 34, "Origin  : " + std::to_string(vOrigin.x) + ", " + std::to_string(vOrigin.y), olc::BLACK);
      return true;
    }
};


int main()
{
  IsometricDemo demo;
  if (demo.Construct(512, 480, 2, 2))
    demo.Start();
  return 0;
}
