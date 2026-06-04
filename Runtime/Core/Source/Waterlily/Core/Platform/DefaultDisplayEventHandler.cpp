#include "Waterlily/Core/Platform/DefaultDisplayEventHandler.hpp"
#include "Waterlily/Core/Platform/WindowHandle.hpp"

namespace Wl
{

    void DefaultDisplayEventHandler::OnWindowClose(const WindowHandle window)
    {
        DisplaySignals::OnWindowClose.Emit(window);
    }

    void DefaultDisplayEventHandler::OnWindowResized(const WindowHandle window, const uint32_t width, const uint32_t height)
    {
        DisplaySignals::OnWindowResized.Emit(window, width, height);
    }

    void DefaultDisplayEventHandler::OnWindowMinimized(const WindowHandle window)
    {
        DisplaySignals::OnWindowMinimized.Emit(window);
    }

    void DefaultDisplayEventHandler::OnWindowShown(const WindowHandle window)
    {
        DisplaySignals::OnWindowShown.Emit(window);
    }

    void DefaultDisplayEventHandler::OnMouseWheel(const float delta)
    {
        Input::OnMouseWheel.Emit(MouseWheel(delta));
    }

    void DefaultDisplayEventHandler::OnMouseMove(float posRelX, float posRelY, float posX, float posY)
    {
        Input::OnMouseMove.Emit(MouseMove(posRelX, posRelY, posX, posY));
    }

    void DefaultDisplayEventHandler::OnButtonUp(Button button)
    {
        Input::s_ButtonsPressed.Remove(button);
        Input::s_ButtonsDown.Remove(button);

        if (!Input::s_ButtonsRelease.Contains(button))
        {
            Input::s_ButtonsRelease.Insert(button);
            Input::OnButtonRelease.Emit(button);
        }

        Input::s_ButtonsUp.Insert(button);
        Input::OnButtonUp(button);
    }

    void DefaultDisplayEventHandler::OnButtonDown(const Button button)
    {
        Input::s_ButtonsRelease.Remove(button);
        Input::s_ButtonsUp.Remove(button);

        if (!Input::s_ButtonsPressed.Contains(button))
        {
            Input::s_ButtonsPressed.Insert(button);
            Input::OnButtonPressed(button);
        }

        Input::s_ButtonsDown.Insert(button);
        Input::OnButtonDown(button);
    }

    void DefaultDisplayEventHandler::OnKeyDown(const VirtualKey key)
    {
        Input::s_KeysRelease.Remove(key);
        Input::s_KeysUp.Remove(key);

        if (!Input::s_KeysPressed.Contains(key))
        {
            Input::s_KeysPressed.Insert(key);
            Input::OnKeyPressed(key);
        }

        Input::s_KeysDow.Insert(key);
        Input::OnKeyDown(key);
    }

    void DefaultDisplayEventHandler::OnKeyUp(const VirtualKey key)
    {
        Input::s_KeysPressed.Remove(key);
        Input::s_KeysDow.Remove(key);

        if (!Input::s_KeysRelease.Contains(key))
        {
            Input::s_KeysRelease.Insert(key);
            Input::OnKeyRelease.Emit(key);
        }

        Input::s_KeysUp.Insert(key);
        Input::OnKeyUp(key);
    }

}// namespace Wl
