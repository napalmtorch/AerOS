#include <apps/app_html.hpp>
#include <core/kernel.hpp>

namespace System
{
    namespace Applications
    {
        Web::Parser *html;
        char* h1string;
        WinHtml::WinHtml() : GUI::Window()
        {
        }

        WinHtml::WinHtml(int32_t x, int32_t y,char* title) : GUI::Window(x, y, 480, 240, title, "htmlviewer")
        {
            
        }

        WinHtml::~WinHtml()
        {

        }

        void WinHtml::OnLoad()
        {
            html = new Web::Parser("file:///sys/web/test.html");
            h1string = html->CheckH1TagString();
            GUI::Window::OnLoad();
        }

        void WinHtml::Update()
        {
            GUI::Window::Update();
        }

        void WinHtml::Draw()
        {
            GUI::Window::Draw();

            if (Flags.CanDraw)
            {
                int32_t dx = ClientBounds.X + 4, dy = ClientBounds.Y + 4;
                Graphics::Canvas::DrawString(dx, dy, h1string, Graphics::Colors::White, Graphics::FONT_8x8_SERIF);
                mem_free(html);
            }
        }
    }
}