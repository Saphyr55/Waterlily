#include "Waterlily/Core/Platform/SDL/SDLInput.hpp"

namespace Wl
{

    Button ButtonFromSDL(int32_t button)
    {
        switch (button)
        {
            case SDL_BUTTON_LEFT:
                return Button::Left;
            case SDL_BUTTON_MIDDLE:
                return Button::Middle;
            case SDL_BUTTON_RIGHT:
                return Button::Right;
            case SDL_BUTTON_X1:
                return Button::XButton1;
            case SDL_BUTTON_X2:
                return Button::XButton2;
            default:
                return Button::Left;
        }
    }

    VirtualKey KeyFromSDL(SDL_Keycode keyCode)
    {
        switch (keyCode)
        {
            case SDLK_ESCAPE:
                return VirtualKey::Escape;
            case SDLK_A:
                return VirtualKey::A;
            case SDLK_B:
                return VirtualKey::B;
            case SDLK_C:
                return VirtualKey::C;
            case SDLK_D:
                return VirtualKey::D;
            case SDLK_E:
                return VirtualKey::E;
            case SDLK_F:
                return VirtualKey::F;
            case SDLK_G:
                return VirtualKey::G;
            case SDLK_H:
                return VirtualKey::H;
            case SDLK_I:
                return VirtualKey::I;
            case SDLK_J:
                return VirtualKey::J;
            case SDLK_K:
                return VirtualKey::K;
            case SDLK_L:
                return VirtualKey::L;
            case SDLK_M:
                return VirtualKey::M;
            case SDLK_N:
                return VirtualKey::N;
            case SDLK_O:
                return VirtualKey::O;
            case SDLK_P:
                return VirtualKey::P;
            case SDLK_Q:
                return VirtualKey::Q;
            case SDLK_R:
                return VirtualKey::R;
            case SDLK_S:
                return VirtualKey::S;
            case SDLK_T:
                return VirtualKey::T;
            case SDLK_U:
                return VirtualKey::U;
            case SDLK_V:
                return VirtualKey::V;
            case SDLK_W:
                return VirtualKey::W;
            case SDLK_X:
                return VirtualKey::X;
            case SDLK_Y:
                return VirtualKey::Y;
            case SDLK_Z:
                return VirtualKey::Z;

            case SDLK_0:
                return VirtualKey::Zero;
            case SDLK_1:
                return VirtualKey::One;
            case SDLK_2:
                return VirtualKey::Two;
            case SDLK_3:
                return VirtualKey::Three;
            case SDLK_4:
                return VirtualKey::Four;
            case SDLK_5:
                return VirtualKey::Five;
            case SDLK_6:
                return VirtualKey::Six;
            case SDLK_7:
                return VirtualKey::Seven;
            case SDLK_8:
                return VirtualKey::Eight;
            case SDLK_9:
                return VirtualKey::Nine;

            case SDLK_F1:
                return VirtualKey::F1;
            case SDLK_F2:
                return VirtualKey::F2;
            case SDLK_F3:
                return VirtualKey::F3;
            case SDLK_F4:
                return VirtualKey::F4;
            case SDLK_F5:
                return VirtualKey::F5;
            case SDLK_F6:
                return VirtualKey::F6;
            case SDLK_F7:
                return VirtualKey::F7;
            case SDLK_F8:
                return VirtualKey::F8;
            case SDLK_F9:
                return VirtualKey::F9;
            case SDLK_F10:
                return VirtualKey::F10;
            case SDLK_F11:
                return VirtualKey::F11;
            case SDLK_F12:
                return VirtualKey::F12;

            case SDLK_LEFT:
                return VirtualKey::Left;
            case SDLK_RIGHT:
                return VirtualKey::Right;
            case SDLK_UP:
                return VirtualKey::Up;
            case SDLK_DOWN:
                return VirtualKey::Down;
            case SDLK_SPACE:
                return VirtualKey::Space;
            case SDLK_RETURN:
                return VirtualKey::Return;
            case SDLK_TAB:
                return VirtualKey::Tab;
            case SDLK_BACKSPACE:
                return VirtualKey::Back;
            case SDLK_MINUS:
                return VirtualKey::Minus;
            case SDLK_EQUALS:
                return VirtualKey::Plus;
            case SDLK_COMMA:
                return VirtualKey::Comma;
            case SDLK_PERIOD:
                return VirtualKey::Period;
            case SDLK_INSERT:
                return VirtualKey::Insert;
            case SDLK_DELETE:
                return VirtualKey::Delete;
            case SDLK_HOME:
                return VirtualKey::Home;
            case SDLK_END:
                return VirtualKey::End;
            case SDLK_PAGEUP:
                return VirtualKey::PageUp;
            case SDLK_PAGEDOWN:
                return VirtualKey::PageDown;

            case SDLK_LSHIFT:
                return VirtualKey::LeftShift;
            case SDLK_RSHIFT:
                return VirtualKey::RightShift;
            case SDLK_LCTRL:
                return VirtualKey::LeftControl;
            case SDLK_RCTRL:
                return VirtualKey::RightControl;
            case SDLK_LALT:
                return VirtualKey::LeftAlt;
            case SDLK_RALT:
                return VirtualKey::RightAlt;
            case SDLK_CAPSLOCK:
                return VirtualKey::CapsLock;

            case SDLK_MUTE:
                return VirtualKey::Mute;
            case SDLK_VOLUMEDOWN:
                return VirtualKey::VolumeDown;
            case SDLK_VOLUMEUP:
                return VirtualKey::VolumeUp;

            case SDLK_KP_PLUS:
                return VirtualKey::KP_Plus;
            case SDLK_KP_MINUS:
                return VirtualKey::KP_Minus;
            case SDLK_KP_MULTIPLY:
                return VirtualKey::KP_Multiply;
            case SDLK_KP_DIVIDE:
                return VirtualKey::KP_Divide;
            case SDLK_KP_ENTER:
                return VirtualKey::KP_Enter;

            case SDLK_KP_1:
                return VirtualKey::KP_1;
            case SDLK_KP_2:
                return VirtualKey::KP_2;
            case SDLK_KP_3:
                return VirtualKey::KP_3;
            case SDLK_KP_4:
                return VirtualKey::KP_4;
            case SDLK_KP_5:
                return VirtualKey::KP_5;
            case SDLK_KP_6:
                return VirtualKey::KP_6;
            case SDLK_KP_7:
                return VirtualKey::KP_7;
            case SDLK_KP_8:
                return VirtualKey::KP_8;
            case SDLK_KP_9:
                return VirtualKey::KP_9;

            case SDLK_KP_0:
                return VirtualKey::KP_0;
            case SDLK_KP_DECIMAL:
                return VirtualKey::KP_Period;

            default:
                return VirtualKey::None;
        }
    }

    int32_t ButtonToSDL(Button b)
    {
        switch (b)
        {
            case Button::Left:
                return SDL_BUTTON_LEFT;// = 1
            case Button::Middle:
                return SDL_BUTTON_MIDDLE;// = 2
            case Button::Right:
                return SDL_BUTTON_RIGHT;// = 3
            case Button::XButton1:
                return SDL_BUTTON_X1;// = 4
            case Button::XButton2:
                return SDL_BUTTON_X2;// = 5
            default: {
                return 0;
            }
        }
    }

    SDL_Keycode KeyToSDL(VirtualKey k)
    {
        switch (k)
        {
            case VirtualKey::A:
                return SDLK_A;
            case VirtualKey::B:
                return SDLK_B;
            case VirtualKey::C:
                return SDLK_C;
            case VirtualKey::D:
                return SDLK_D;
            case VirtualKey::E:
                return SDLK_E;
            case VirtualKey::F:
                return SDLK_F;
            case VirtualKey::G:
                return SDLK_G;
            case VirtualKey::H:
                return SDLK_H;
            case VirtualKey::I:
                return SDLK_I;
            case VirtualKey::J:
                return SDLK_J;
            case VirtualKey::K:
                return SDLK_K;
            case VirtualKey::L:
                return SDLK_L;
            case VirtualKey::M:
                return SDLK_M;
            case VirtualKey::N:
                return SDLK_N;
            case VirtualKey::O:
                return SDLK_O;
            case VirtualKey::P:
                return SDLK_P;
            case VirtualKey::Q:
                return SDLK_Q;
            case VirtualKey::R:
                return SDLK_R;
            case VirtualKey::S:
                return SDLK_S;
            case VirtualKey::T:
                return SDLK_T;
            case VirtualKey::U:
                return SDLK_U;
            case VirtualKey::V:
                return SDLK_V;
            case VirtualKey::W:
                return SDLK_W;
            case VirtualKey::X:
                return SDLK_X;
            case VirtualKey::Y:
                return SDLK_Y;
            case VirtualKey::Z:
                return SDLK_Z;

            case VirtualKey::Zero:
                return SDLK_0;
            case VirtualKey::One:
                return SDLK_1;
            case VirtualKey::Two:
                return SDLK_2;
            case VirtualKey::Three:
                return SDLK_3;
            case VirtualKey::Four:
                return SDLK_4;
            case VirtualKey::Five:
                return SDLK_5;
            case VirtualKey::Six:
                return SDLK_6;
            case VirtualKey::Seven:
                return SDLK_7;
            case VirtualKey::Eight:
                return SDLK_8;
            case VirtualKey::Nine:
                return SDLK_9;

            case VirtualKey::F1:
                return SDLK_F1;
            case VirtualKey::F2:
                return SDLK_F2;
            case VirtualKey::F3:
                return SDLK_F3;
            case VirtualKey::F4:
                return SDLK_F4;
            case VirtualKey::F5:
                return SDLK_F5;
            case VirtualKey::F6:
                return SDLK_F6;
            case VirtualKey::F7:
                return SDLK_F7;
            case VirtualKey::F8:
                return SDLK_F8;
            case VirtualKey::F9:
                return SDLK_F9;
            case VirtualKey::F10:
                return SDLK_F10;
            case VirtualKey::F11:
                return SDLK_F11;
            case VirtualKey::F12:
                return SDLK_F12;

            case VirtualKey::Left:
                return SDLK_LEFT;
            case VirtualKey::Right:
                return SDLK_RIGHT;
            case VirtualKey::Up:
                return SDLK_UP;
            case VirtualKey::Down:
                return SDLK_DOWN;
            case VirtualKey::Space:
                return SDLK_SPACE;
            case VirtualKey::Return:
                return SDLK_RETURN;
            case VirtualKey::Tab:
                return SDLK_TAB;
            case VirtualKey::Back:
                return SDLK_BACKSPACE;
            case VirtualKey::Minus:
                return SDLK_MINUS;
            case VirtualKey::Plus:
                return SDLK_PLUS;
            case VirtualKey::Comma:
                return SDLK_COMMA;
            case VirtualKey::Period:
                return SDLK_PERIOD;
            case VirtualKey::Insert:
                return SDLK_INSERT;
            case VirtualKey::Delete:
                return SDLK_DELETE;
            case VirtualKey::Home:
                return SDLK_HOME;
            case VirtualKey::End:
                return SDLK_END;
            case VirtualKey::PageUp:
                return SDLK_PAGEUP;
            case VirtualKey::PageDown:
                return SDLK_PAGEDOWN;

            case VirtualKey::LeftShift:
                return SDLK_LSHIFT;
            case VirtualKey::RightShift:
                return SDLK_RSHIFT;
            case VirtualKey::LeftControl:
                return SDLK_LCTRL;
            case VirtualKey::RightControl:
                return SDLK_RCTRL;
            case VirtualKey::LeftAlt:
                return SDLK_LALT;
            case VirtualKey::RightAlt:
                return SDLK_RALT;
            case VirtualKey::CapsLock:
                return SDLK_CAPSLOCK;
            case VirtualKey::Mute:
                return SDLK_MUTE;
            case VirtualKey::VolumeDown:
                return SDLK_VOLUMEDOWN;
            case VirtualKey::VolumeUp:
                return SDLK_VOLUMEUP;
            case VirtualKey::KP_Plus:
                return SDLK_KP_PLUS;
            case VirtualKey::KP_Minus:
                return SDLK_KP_MINUS;
            case VirtualKey::KP_Multiply:
                return SDLK_KP_MULTIPLY;
            case VirtualKey::KP_Divide:
                return SDLK_KP_DIVIDE;
            case VirtualKey::KP_Enter:
                return SDLK_KP_ENTER;

            case VirtualKey::KP_1:
                return SDLK_KP_1;
            case VirtualKey::KP_2:
                return SDLK_KP_2;
            case VirtualKey::KP_3:
                return SDLK_KP_3;
            case VirtualKey::KP_4:
                return SDLK_KP_4;
            case VirtualKey::KP_5:
                return SDLK_KP_5;
            case VirtualKey::KP_6:
                return SDLK_KP_6;
            case VirtualKey::KP_7:
                return SDLK_KP_7;
            case VirtualKey::KP_8:
                return SDLK_KP_8;
            case VirtualKey::KP_9:
                return SDLK_KP_9;

                // case Key::KP_PERIOD: return SDLK_KP_DECIMAL;

            default:
                return SDLK_UNKNOWN;
        }
    }

}// namespace Wl
