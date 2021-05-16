#include "apps/win_demo.hpp"

namespace System
{
    namespace Applications
    {
        GUI::Button* DemoBtn;
        GUI::CheckBox* DemoChk;
        GUI::TextBox * DemoText;
        char checked_str[] = "Checked";
        char unchecked_str[] = "Unchecked";

        WinGUIDemo::WinGUIDemo()
        {

        }

        WinGUIDemo::WinGUIDemo(int32_t x, int32_t y) : GUI::Window(x, y, 320, 200, "GUI Demo")
        {
            DemoBtn = new GUI::Button(ClientBounds->X + 4, ClientBounds->Y + 4, "Click me");
            DemoChk = new GUI::CheckBox(ClientBounds->X + 4, ClientBounds->Y + 30, "Unchecked");
            DemoText = new GUI::TextBox(ClientBounds->X + 4, ClientBounds->Y + 56);
        }

        void WinGUIDemo::Update()
        {
            GUI::Window::Update();

            if (Flags->CanDraw)
            {
                // update button
                DemoBtn->SetPosition(ClientBounds->X + 4, ClientBounds->Y + 4);
                DemoBtn->Update();
                
                // update checkbox
                DemoChk->SetPosition(ClientBounds->X + 4, ClientBounds->Y + 30);
                DemoChk->Update();
                if (DemoChk->Flags->Toggled) { DemoChk->Text = checked_str; } else { DemoChk->Text = unchecked_str; }

                // update textbox
                DemoText->SetPosition(ClientBounds->X + 4, ClientBounds->Y + 56);
                DemoText->Update();
            }
        }

        void WinGUIDemo::Draw()
        {
            GUI::Window::Draw();

            if (Flags->CanDraw)
            {
                // draw widgets
                DemoBtn->Draw();
                DemoChk->Draw();
                DemoText->Draw();
            }
        }
    }
}