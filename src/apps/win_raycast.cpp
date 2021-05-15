#include "apps/win_raycast.hpp"
#include <core/kernel.hpp>

int abs(int i) { return i < 0 ? -i : i; }

namespace System
{
    namespace Applications
    {
        const int map_width = 24;
        const int map_height = 24;
        int map[map_width][map_height]=
        {
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
            {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
            {1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
            {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
        };

        double posX = 22, posY = 12;  //x and y start position
        double dirX = -1, dirY = 0; //initial direction vector
        double planeX = 0, planeY = 0.66; //the 2d raycaster version of camera plane

        uint32_t oldw, oldh;
        

        WinRaycaster::WinRaycaster()
        {

        }

        WinRaycaster::WinRaycaster(int32_t x, int32_t y) : GUI::Window(x, y, 320, 200, "Raycaster")
        {
            BufferWidth = ClientBounds->Width;
            BufferHeight = ClientBounds->Height;
            Buffer = new uint8_t[BufferWidth * BufferHeight];
            oldw = BufferWidth;
            oldh = BufferHeight;

            Style = GUI::CopyStyle(&GUI::WindowStyle);
            Style->Colors[0] = Graphics::Colors::Transparent;
        }

        void WinRaycaster::Update()
        {
            // update base
            GUI::Window::Update();

            double moveSpeed = 0.05;
            double rotSpeed = 0.05;

            // move forward
            if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::W))
            {
                if(map[int(posX + dirX * moveSpeed)][int(posY)] == false) posX += dirX * moveSpeed;
                if(map[int(posX)][int(posY + dirY * moveSpeed)] == false) posY += dirY * moveSpeed;
            }

            // move backward
            if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::S))
            {
                if(map[int(posX - dirX * moveSpeed)][int(posY)] == false) posX -= dirX * moveSpeed;
                if(map[int(posX)][int(posY - dirY * moveSpeed)] == false) posY -= dirY * moveSpeed;
            }

            // turn left
            if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::A))
            {
               
            }

            // turn right
            if (KernelIO::Keyboard.IsKeyDown(HAL::Keys::D))
            {
               
            }

            if (oldw != Bounds->Width || oldh != Bounds->Height)
            {
                if (Buffer != nullptr) { delete Buffer; }

                // initialize buffer
                BufferWidth = ClientBounds->Width;
                BufferHeight = ClientBounds->Height;
                Buffer = new uint8_t[BufferWidth * BufferHeight];

                oldw = Bounds->Width;
                oldh = Bounds->Height;
            }

            // clear buffer
            for (size_t i = 0; i < BufferWidth * BufferHeight; i++) { Buffer[i] = 0; }

            // raycast
            for(int x = 0; x < BufferWidth; x++)
            {
                //calculate ray position and direction
                double cameraX = 2 * x / (double)BufferWidth - 1; //x-coordinate in camera space
                double rayDirX = dirX + planeX * cameraX;
                double rayDirY = dirY + planeY * cameraX;
                //which box of the map we're in
                int mapX = int(posX);
                int mapY = int(posY);

                //length of ray from current position to next x or y-side
                double sideDistX;
                double sideDistY;

                //length of ray from one x or y-side to next x or y-side
                double deltaDistX = abs(1 / rayDirX);
                double deltaDistY = abs(1 / rayDirY);
                double perpWallDist;

                //what direction to step in x or y-direction (either +1 or -1)
                int stepX;
                int stepY;

                int hit = 0; //was there a wall hit?
                int side; //was a NS or a EW wall hit?
                //calculate step and initial sideDist
                if(rayDirX < 0)
                {
                    stepX = -1;
                    sideDistX = (posX - mapX) * deltaDistX;
                }
                else
                {
                    stepX = 1;
                    sideDistX = (mapX + 1.0 - posX) * deltaDistX;
                }
                if(rayDirY < 0)
                {
                    stepY = -1;
                    sideDistY = (posY - mapY) * deltaDistY;
                }
                else
                {
                    stepY = 1;
                    sideDistY = (mapY + 1.0 - posY) * deltaDistY;
                }
                //perform DDA
                while (hit == 0)
                {
                    //jump to next map square, OR in x-direction, OR in y-direction
                    if(sideDistX < sideDistY)
                    {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    side = 0;
                    }
                    else
                    {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    side = 1;
                    }
                    //Check if ray has hit a wall
                    if(map[mapX][mapY] > 0) hit = 1;
                }
                //Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
                if(side == 0) perpWallDist = (mapX - posX + (1 - stepX) / 2) / rayDirX;
                else          perpWallDist = (mapY - posY + (1 - stepY) / 2) / rayDirY;

                //Calculate height of line to draw on screen
                int lineHeight = (int)(BufferHeight / perpWallDist);

                //calculate lowest and highest pixel to fill in current stripe
                int drawStart = -lineHeight / 2 + BufferHeight / 2;
                if(drawStart < 0)drawStart = 0;
                int drawEnd = lineHeight / 2 + BufferHeight / 2;
                if(drawEnd >= BufferHeight)drawEnd = BufferHeight - 1;

                //choose wall color
                uint8_t color;
                switch(map[mapX][mapY])
                {
                    case 1:  color = COL4_RED;    break; //red
                    case 2:  color = COL4_GREEN;  break; //green
                    case 3:  color = COL4_BLUE;   break; //blue
                    case 4:  color = COL4_YELLOW; break; //white
                    default: color = COL4_WHITE;  break; //yellow
                }

                //give x and y sides different brightness
                if(side == 1) {color = color -= 8;}

                //draw the pixels of the stripe as a vertical line
                for (size_t i = drawStart; i < drawEnd; i++)
                {
                    uint32_t pos = x + (i * BufferWidth);
                    Buffer[pos] = color;
                }
            }
        }

        void WinRaycaster::Draw()
        {
            // draw base
            GUI::Window::Draw();

            if (Flags->CanDraw)
            {
                for (size_t yy = 0; yy < BufferHeight; yy++)
                {
                    for (size_t xx = 0; xx < BufferWidth; xx++)
                    {
                        uint32_t offset = xx + (yy * BufferWidth);
                        Graphics::Canvas::DrawPixel(ClientBounds->X + xx, ClientBounds->Y + yy, ConvertColor(Buffer[offset]));   
                    }
                }
            }
        }

        Color WinRaycaster::ConvertColor(uint8_t color)
        {
            switch (color)
            {
                case 0x00: { return Graphics::Colors::Black; break; }
                case 0x01: { return Graphics::Colors::DarkBlue; break; }
                case 0x02: { return Graphics::Colors::DarkGreen; break; }
                case 0x03: { return Graphics::Colors::DarkCyan; break; }
                case 0x04: { return Graphics::Colors::DarkRed; break; }
                case 0x05: { return Graphics::Colors::DarkMagenta; break; }
                case 0x06: { return Graphics::Colors::DarkOrange; break; }
                case 0x07: { return Graphics::Colors::DarkGray; break; }
                case 0x08: { return Graphics::Colors::DarkSlateBlue; break; }
                case 0x09: { return Graphics::Colors::Blue; break; }
                case 0x0A: { return Graphics::Colors::Green; break; }
                case 0x0B: { return Graphics::Colors::Cyan; break; }
                case 0x0C: { return Graphics::Colors::Red; break; }
                case 0x0D: { return Graphics::Colors::Magenta; break; }
                case 0x0E: { return Graphics::Colors::Yellow; break; }
                case 0x0F: { return Graphics::Colors::White; break; }
                default: return Graphics::Colors::Black;
            }

            return Graphics::Colors::Black;
        }
    }
}