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
            void Initialize();
            void Start();
            void Update();
            void Draw();
            bool IsRunning();
        private:
            Graphics::VGACanvas Canvas;
            bool Running;
            uint32_t FPS, frames, time, last_time;
    };
}