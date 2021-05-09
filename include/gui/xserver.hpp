#pragma once
#include <lib/types.h>
#include <lib/string.hpp>
#include <hardware/ports.hpp>
#include <graphics/canvas.hpp>

namespace System
{
    class XServerHost
    {
        public:
            Graphics::VGACanvas Canvas;
            Graphics::Canvas FullCanvas;
            void Initialize();
            void Start(VIDEO_DRIVER driver);
            void Update();
            void Draw();
            bool IsRunning();
        private:
            bool Running;
            uint32_t FPS, frames, time, last_time;
    };
}