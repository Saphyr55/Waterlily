#pragma once

#include "Waterlily/Core/Platform/Input.hpp"

#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>

namespace Wl
{

    Button ButtonFromSDL(int32_t buttonCode);

    VirtualKey KeyFromSDL(SDL_Keycode keyCode);

    int32_t ButtonToSDL(Button button);

    SDL_Keycode KeyToSDL(VirtualKey key);

}// namespace Wl